/***********************************************************************************
This file is part of Project for Meloton
For the latest info, see  https://github.com/Yhgenomics/MelotonMaster.git

Copyright 2016 Yhgenomics

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***********************************************************************************/

/***********************************************************************************
* Description   :
* Creator       : Shubo Yang
* Date          :
* Modifed       : When      | Who       | What
***********************************************************************************/

#ifndef MASTER_SESSION_H_
#define MASTER_SESSION_H_

#include <MRT.h>
#include <MelotonSession.h>

class MasterSession : 
    public MelotonSession
{
public:

    static MasterSession* Instance();
    static void           SetInstance( MasterSession* session );
    MasterSession ();
    ~MasterSession();

protected:

     void OnConnect  () override;

private:

    static MasterSession* instance_;
    MRT::SyncWorker* alive_worker_  = nullptr;


};

#endif // !MASTER_SESSION_H_ 