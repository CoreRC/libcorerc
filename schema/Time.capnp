@0xe3ccb5f5fae08819;


using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("corerc");


struct Time {
    sec @0 : Int64;
    nsec @1 : Int64;
}
