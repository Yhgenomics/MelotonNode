#include "DuplicateSession.h"
#include <MessageDuplicateDataRequest.pb.h>
#include <BlockHub.h>
#include <MRT.h>
#include <MasterSession.h>
#include <MessageSyncBlock.pb.h>
#include <DuplicateConnector.h>

DuplicateSession::DuplicateSession()
{ 

}

DuplicateSession::DuplicateSession( uptr<MessageDuplicateBlock> msg )
{
    this->message_block_ = move_ptr( msg );

    this->path_          = this->message_block_->path();
    this->part_id_       = this->message_block_->partid();
    this->file_offset_   = this->message_block_->fileoffset();
    this->remote_index_  = this->message_block_->index();
    this->remote_ip_     = this->message_block_->address();

    this->index_ = BlockHub::Instance()->FindBlock( this->path_ , 
                                                    this->part_id_ );
    
    if(this->index_ == nullptr )
        this->index_ = BlockHub::Instance()->CreateBlock( (int)this->part_id_ ,
                                                          this->file_offset_ ,
                                                          this->path_ ); 
}

DuplicateSession::~DuplicateSession()
{ 
    if ( this->worker_ != nullptr )
    {
        MRT::SyncWorker::Stop( this->worker_ );
    }
}

void DuplicateSession::SendRequest()
{
    uptr<MessageDuplicateDataRequest> message = make_uptr( MessageDuplicateDataRequest );
    message->set_index     ( this->remote_index_ );
    message->set_token     ( "" );
    message->set_offset    ( this->block_offset_ );
    message->set_size      ( BLOCK_TRANSFER_SIZE );
    message->set_sessionid ( this->Id() );
    this->SendMessage      ( move_ptr( message ) );
}

void DuplicateSession::OnConnect()
{

}

bool DuplicateSession::DuplicateFinish()
{
    return this->finish_;
}

void DuplicateSession::RetryTimer()
{
    if ( this->worker_ != nullptr )
    {
        MRT::SyncWorker::Stop( this->worker_ );
    }

    this->worker_ = MRT::SyncWorker::Create( 15000 , [] ( MRT::SyncWorker* worker ) {
        
        DuplicateSession* session = (DuplicateSession*) worker->Data();
        
        Logger::Log( "retring duplicate path % part % from %" , 
                     session->message_block_->path() , 
                     session->message_block_->partid() , 
                     session->message_block_->address() );

        sptr<DuplicateConnector> connector = make_sptr( DuplicateConnector , move_ptr( session->message_block_ ) );
        MRT::Maraton::Instance()->Regist( connector );

        session->Close();

        return true;

    } , nullptr , this );
}

void DuplicateSession::AcceptBlock( uptr<MessageDuplicateData> msg )
{
    auto wsize = BlockHub::Instance()->WriteBlock( this->index_->Index ,
                                                   msg->offset() ,
                                                   msg->data().c_str() ,
                                                   msg->data().size() );
    this->index_->Size = msg->offset() + wsize;
    
    if ( msg->islast() )
    {  
        BlockHub::Instance()->SaveBlockIndex( this->index_ );

        auto sync = make_uptr   ( MessageBlockMeta );
        sync->set_fileoffset    ( this->index_->FileOffset );
        sync->set_index         ( this->index_->Index );
        sync->set_partid        ( this->index_->PartId );
        sync->set_path          ( this->index_->Path );
        sync->set_size          ( this->index_->Size );
        sync->set_status        ( 0 );
        MasterSession::Instance ()->SendMessage( move_ptr( sync ) );

        Logger::Log( "duplicate path % part:% size:% from %" ,
                     this->index_->Path ,
                     this->index_->PartId ,
                     this->index_->Size ,
                     this->remote_ip_ );

        if ( this->worker_ != nullptr )
        {
            MRT::SyncWorker::Stop( this->worker_ );
        }

        this->finish_ = true; 
        this->Close();
        return;
    }

    this->block_offset_ = msg->offset() + wsize;
    this->SendRequest();
}
