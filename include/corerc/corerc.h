#ifndef _CORERC_H
#define _CORERC_H

#include <kj/thread.h>
#include <kj/async-unix.h>
#include <kj/async-io.h>

namespace corerc {

    inline kj::AsyncIoContext setupAsyncIo(){
        return kj::setupAsyncIo();
    }

    inline static void captureSignal(int signum){
        kj::UnixEventPort::captureSignal(signum);
    }

};


#endif