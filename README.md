# Pub/Sub in C++ Cap'n Proto

Client for the Rust-based server example in `capnproto-rust`.

# See also

My [blog](https://corerc.io/blog/2018/08/31/first-cross-language-pub-sub-on-capn-proto/).

# Building

```
mkdir build
cd $_
cmake ..
make -j
```

# Running

First kick up the server, then:

```
./test_capnp client
```

# LICENSE

MIT
