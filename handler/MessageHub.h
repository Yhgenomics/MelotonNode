#ifndef MESSAGE_HUB_H_
#define MESSAGE_HUB_H_

#include <string>
#include <memory>
#include <MRT.h>
#include <google/protobuf/message.h>
#include "MessageAliveHandler.h"
#include "MessageAlive.pb.h"
#include "MessageBlockAcceptHandler.h"
#include "MessageBlockAccept.pb.h"
#include "MessageBlockDataHandler.h"
#include "MessageBlockData.pb.h"
#include "MessageBlockListHandler.h"
#include "MessageBlockList.pb.h"
#include "MessageBlockMetaHandler.h"
#include "MessageBlockMeta.pb.h"
#include "MessageDeleteBlockHandler.h"
#include "MessageDeleteBlock.pb.h"
#include "MessageDeleteBlockACKHandler.h"
#include "MessageDeleteBlockACK.pb.h"
#include "MessageDeleteFileHandler.h"
#include "MessageDeleteFile.pb.h"
#include "MessageDeleteFileACKHandler.h"
#include "MessageDeleteFileACK.pb.h"
#include "MessageDuplicateBlockHandler.h"
#include "MessageDuplicateBlock.pb.h"
#include "MessageDuplicateDataHandler.h"
#include "MessageDuplicateData.pb.h"
#include "MessageDuplicateDataAcceptHandler.h"
#include "MessageDuplicateDataAccept.pb.h"
#include "MessageDuplicateDataRequestHandler.h"
#include "MessageDuplicateDataRequest.pb.h"
#include "MessageErrorHandler.h"
#include "MessageError.pb.h"
#include "MessageNewBlockHandler.h"
#include "MessageNewBlock.pb.h"
#include "MessageOpenACKHandler.h"
#include "MessageOpenACK.pb.h"
#include "MessageOpenFileHandler.h"
#include "MessageOpenFile.pb.h"
#include "MessagePrepareReadHandler.h"
#include "MessagePrepareRead.pb.h"
#include "MessagePrepareReadACKHandler.h"
#include "MessagePrepareReadACK.pb.h"
#include "MessagePrepareWriteHandler.h"
#include "MessagePrepareWrite.pb.h"
#include "MessagePrepareWriteACKHandler.h"
#include "MessagePrepareWriteACK.pb.h"
#include "MessageReadHandler.h"
#include "MessageRead.pb.h"
#include "MessageReadBlockHandler.h"
#include "MessageReadBlock.pb.h"
#include "MessageSyncBlockHandler.h"
#include "MessageSyncBlock.pb.h"
#include "MessageTellHandler.h"
#include "MessageTell.pb.h"
#include "MessageTellACKHandler.h"
#include "MessageTellACK.pb.h"
#include "MessageWriteHandler.h"
#include "MessageWrite.pb.h"


using namespace MRT;

class MessageHub
{
private:

    static size_t hash_name( std::string name)
    {
        size_t ret = 0;
        for (int i = 0; i < name.length(); i++)
        {
            char b = (char)name[i];
            size_t v = (((size_t)b << ((i % (char)8) * (char)8)) | i);
            ret |= (size_t)(v);
        }
        return ret;
    }
public:

    static uptr<MRT::Buffer> Build(::google::protobuf::Message * message)
    {
        size_t message_id = hash_name( message->GetTypeName( ) );
        std::string body = message->SerializeAsString( );
        MRT::Buffer buffer(body.size( ) + sizeof( size_t ));
        char* pbuf = buffer.Data( );
        *((size_t*)pbuf) = message_id;
        pbuf += sizeof( size_t );
        memcpy(pbuf , body.c_str( ) , body.size( ) );
        return make_uptr( MRT::Buffer , buffer );
    }

    static int Handle( Session * session , const void* pdata , size_t len )
    {
        size_t message_id = 0;
        char* data = ( char* )pdata;
        message_id = *( (size_t*) data );
        data += sizeof( size_t );
        int msg_len = scast<int>( len - sizeof( size_t ) );

        switch( message_id )
        {
        case 0x4165676177776D6F : 
            {
                auto msg = new MessageAlive( );
                msg->ParseFromArray( data, msg_len );
                return MessageAliveHandler( session , std::move( std::unique_ptr<MessageAlive>( msg ) ) );
            }break;
        case 0x676767617B737F7F : 
            {
                auto msg = new MessageBlockAccept( );
                msg->ParseFromArray( data, msg_len );
                return MessageBlockAcceptHandler( session , std::move( std::unique_ptr<MessageBlockAccept>( msg ) ) );
            }break;
        case 0x637567657B736F6F : 
            {
                auto msg = new MessageBlockData( );
                msg->ParseFromArray( data, msg_len );
                return MessageBlockDataHandler( session , std::move( std::unique_ptr<MessageBlockData>( msg ) ) );
            }break;
        case 0x76776F6D7B736F6F : 
            {
                auto msg = new MessageBlockList( );
                msg->ParseFromArray( data, msg_len );
                return MessageBlockListHandler( session , std::move( std::unique_ptr<MessageBlockList>( msg ) ) );
            }break;
        case 0x6375676D7B736F6F : 
            {
                auto msg = new MessageBlockMeta( );
                msg->ParseFromArray( data, msg_len );
                return MessageBlockMetaHandler( session , std::move( std::unique_ptr<MessageBlockMeta>( msg ) ) );
            }break;
        case 0x6F6D676577776F7F : 
            {
                auto msg = new MessageDeleteBlock( );
                msg->ParseFromArray( data, msg_len );
                return MessageDeleteBlockHandler( session , std::move( std::unique_ptr<MessageDeleteBlock>( msg ) ) );
            }break;
        case 0x6F6D676F77776F7F : 
            {
                auto msg = new MessageDeleteBlockACK( );
                msg->ParseFromArray( data, msg_len );
                return MessageDeleteBlockACKHandler( session , std::move( std::unique_ptr<MessageDeleteBlockACK>( msg ) ) );
            }break;
        case 0x6C6D676577776D7F : 
            {
                auto msg = new MessageDeleteFile( );
                msg->ParseFromArray( data, msg_len );
                return MessageDeleteFileHandler( session , std::move( std::unique_ptr<MessageDeleteFile>( msg ) ) );
            }break;
        case 0x6C6D67657F776D7F : 
            {
                auto msg = new MessageDeleteFileACK( );
                msg->ParseFromArray( data, msg_len );
                return MessageDeleteFileACKHandler( session , std::move( std::unique_ptr<MessageDeleteFileACK>( msg ) ) );
            }break;
        case 0x6575676B7B7F7D7F : 
            {
                auto msg = new MessageDuplicateBlock( );
                msg->ParseFromArray( data, msg_len );
                return MessageDuplicateBlockHandler( session , std::move( std::unique_ptr<MessageDuplicateBlock>( msg ) ) );
            }break;
        case 0x657567637B7F757F : 
            {
                auto msg = new MessageDuplicateData( );
                msg->ParseFromArray( data, msg_len );
                return MessageDuplicateDataHandler( session , std::move( std::unique_ptr<MessageDuplicateData>( msg ) ) );
            }break;
        case 0x657767637B7F757F : 
            {
                auto msg = new MessageDuplicateDataAccept( );
                msg->ParseFromArray( data, msg_len );
                return MessageDuplicateDataAcceptHandler( session , std::move( std::unique_ptr<MessageDuplicateDataAccept>( msg ) ) );
            }break;
        case 0x757567737B7F777F : 
            {
                auto msg = new MessageDuplicateDataRequest( );
                msg->ParseFromArray( data, msg_len );
                return MessageDuplicateDataRequestHandler( session , std::move( std::unique_ptr<MessageDuplicateDataRequest>( msg ) ) );
            }break;
        case 0x45656761737F777F : 
            {
                auto msg = new MessageError( );
                msg->ParseFromArray( data, msg_len );
                return MessageErrorHandler( session , std::move( std::unique_ptr<MessageError>( msg ) ) );
            }break;
        case 0x4E6F676F7F73776F : 
            {
                auto msg = new MessageNewBlock( );
                msg->ParseFromArray( data, msg_len );
                return MessageNewBlockHandler( session , std::move( std::unique_ptr<MessageNewBlock>( msg ) ) );
            }break;
        case 0x4F656F63737F657F : 
            {
                auto msg = new MessageOpenACK( );
                msg->ParseFromArray( data, msg_len );
                return MessageOpenACKHandler( session , std::move( std::unique_ptr<MessageOpenACK>( msg ) ) );
            }break;
        case 0x4F656F69777F657F : 
            {
                auto msg = new MessageOpenFile( );
                msg->ParseFromArray( data, msg_len );
                return MessageOpenFileHandler( session , std::move( std::unique_ptr<MessageOpenFile>( msg ) ) );
            }break;
        case 0x757767737373657F : 
            {
                auto msg = new MessagePrepareRead( );
                msg->ParseFromArray( data, msg_len );
                return MessagePrepareReadHandler( session , std::move( std::unique_ptr<MessagePrepareRead>( msg ) ) );
            }break;
        case 0x7577677B7373657F : 
            {
                auto msg = new MessagePrepareReadACK( );
                msg->ParseFromArray( data, msg_len );
                return MessagePrepareReadACKHandler( session , std::move( std::unique_ptr<MessagePrepareReadACK>( msg ) ) );
            }break;
        case 0x727767737377757F : 
            {
                auto msg = new MessagePrepareWrite( );
                msg->ParseFromArray( data, msg_len );
                return MessagePrepareWriteHandler( session , std::move( std::unique_ptr<MessagePrepareWrite>( msg ) ) );
            }break;
        case 0x72776F737377757F : 
            {
                auto msg = new MessagePrepareWriteACK( );
                msg->ParseFromArray( data, msg_len );
                return MessagePrepareWriteACKHandler( session , std::move( std::unique_ptr<MessagePrepareWriteACK>( msg ) ) );
            }break;
        case 0x526567617377656F : 
            {
                auto msg = new MessageRead( );
                msg->ParseFromArray( data, msg_len );
                return MessageReadHandler( session , std::move( std::unique_ptr<MessageRead>( msg ) ) );
            }break;
        case 0x7B676F6D7377656F : 
            {
                auto msg = new MessageReadBlock( );
                msg->ParseFromArray( data, msg_len );
                return MessageReadBlockHandler( session , std::move( std::unique_ptr<MessageReadBlock>( msg ) ) );
            }break;
        case 0x7B676F6D73736F7F : 
            {
                auto msg = new MessageSyncBlock( );
                msg->ParseFromArray( data, msg_len );
                return MessageSyncBlockHandler( session , std::move( std::unique_ptr<MessageSyncBlock>( msg ) ) );
            }break;
        case 0x54656761737F6D6F : 
            {
                auto msg = new MessageTell( );
                msg->ParseFromArray( data, msg_len );
                return MessageTellHandler( session , std::move( std::unique_ptr<MessageTell>( msg ) ) );
            }break;
        case 0x54656F63737F6D6F : 
            {
                auto msg = new MessageTellACK( );
                msg->ParseFromArray( data, msg_len );
                return MessageTellACKHandler( session , std::move( std::unique_ptr<MessageTellACK>( msg ) ) );
            }break;
        case 0x5765676177776D7F : 
            {
                auto msg = new MessageWrite( );
                msg->ParseFromArray( data, msg_len );
                return MessageWriteHandler( session , std::move( std::unique_ptr<MessageWrite>( msg ) ) );
            }break;
     default:
                {
                    return -1;
                }break;      }
        return -1;
    }
};

#endif // !MESSAGE_HUB_H_
