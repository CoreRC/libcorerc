@0xca2fef41f2cdf0cd;


using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("corerc");

struct Message(T) {
    stamp @0 : import "Time.capnp".Time;
    frameId @1 : Text;
    data @2 : T;
}
