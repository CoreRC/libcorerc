#include <corerc/pubsub.h>
#include <iostream>
#include <sys/time.h>

class SubscriberImpl final : public corerc::Subscriber<capnp::Text>::Server {
public:

    kj::Promise<void> pushMessage(PushMessageContext context) override {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        std::cout << "Message From Publisher: " << context.getParams().getMessage().cStr() << tv.tv_usec << std::endl;
        std::cout.flush();
        return kj::READY_NOW;
    }
};

void writeAddressBook(int fd) {
}

void startClient() {
    corerc::captureSignal(SIGINT);
    auto ioContext = corerc::setupAsyncIo();

    auto addrPromise = ioContext.provider->getNetwork().parseAddress("127.0.0.1", 2572)
            .then([](kj::Own<kj::NetworkAddress> addr) {
                std::cerr << "Got an address." << std::endl;
                return addr->connect().attach(kj::mv(addr));
            });

    auto stream = addrPromise.wait(ioContext.waitScope);

    capnp::TwoPartyVatNetwork network(*stream, capnp::rpc::twoparty::Side::CLIENT);

    auto rpcSystem = capnp::makeRpcClient(network);

    std::cerr << "Returning rpcSystem" << std::endl;
    {
        capnp::MallocMessageBuilder message(4);
        auto hostId = message.getRoot<capnp::rpc::twoparty::VatId>();
        hostId.setSide(capnp::rpc::twoparty::Side::SERVER);

        corerc::Publisher<capnp::Text>::Client publisher = rpcSystem.bootstrap(
                hostId).castAs<corerc::Publisher<capnp::Text>>();

        std::cerr << "Creating Client" << std::endl;

        corerc::Subscriber<capnp::Text>::Client sub = corerc::Subscriber<capnp::Text>::Client(
                kj::heap<SubscriberImpl>());

        capnp::Request<corerc::Publisher<capnp::Text>::SubscribeParams, corerc::Publisher<capnp::Text>::SubscribeResults> request = publisher.subscribeRequest();

        request.setSubscriber(sub);
        request.setTopic("/object1/position");

        capnp::RemotePromise<corerc::Publisher<capnp::Text>::SubscribeResults> subscriptionPromise = request.send();

        capnp::Response<corerc::Publisher<capnp::Text>::SubscribeResults> subscription = subscriptionPromise.wait(
                ioContext.waitScope);

        ioContext.unixEventPort.onSignal(SIGINT).wait(ioContext.waitScope);
    }

    capnp::MallocMessageBuilder message;
    auto hostId = message.getRoot<capnp::rpc::twoparty::VatId>();
    hostId.setSide(capnp::rpc::twoparty::Side::SERVER);

    corerc::Publisher<capnp::Text>::Client publisher = rpcSystem.bootstrap(
            hostId).castAs<corerc::Publisher<capnp::Text>>();

    std::cerr << "Creating Client" << std::endl;

    auto sub = corerc::Subscriber<capnp::Text>::Client(kj::heap<SubscriberImpl>());

    auto request = publisher.subscribeRequest();
    std::cerr << "Received SIGINT, dropping connection!" << std::endl;

    auto sub1 = corerc::Subscriber<capnp::Text>::Client(kj::heap<SubscriberImpl>());

    std::cerr << "request.setSubscriber(*sub1);!" << std::endl;
    request.setSubscriber(sub1);
    request.setTopic("/object1/position");
    std::cerr << "auto subscription1 = request.send().wait(ioContext.waitScope);" << std::endl;
    
    corerc::Subscription::Client sub_obj(nullptr);
    auto subsresult = request.send().then(
            [&sub_obj](capnp::Response<corerc::Publisher<capnp::Text>::SubscribeResults> resp) {
                sub_obj = resp.getSubscription();
                return;
            }, [](kj::Exception &&e) {
                std::cerr << "Failed to get Subscription" << std::endl;
                return;
            });

    subsresult.wait(ioContext.waitScope);
    auto& timer = ioContext.unixEventPort.getTimer();
    
    auto drop = timer.afterDelay(2 * kj::SECONDS).then(
            [&sub_obj]() {
                std::cerr << "Dropping Subscription" << std::endl;
                sub_obj = nullptr;
                return;
            }).eagerlyEvaluate(nullptr);
    
    drop.wait(ioContext.waitScope);
    // ioContext.unixEventPort.onSignal(SIGINT).wait(ioContext.waitScope);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Missing arg." << std::endl;
        return 1;
    } else if (strcmp(argv[1], "write") == 0) {
        writeAddressBook(1);
    } else if (strcmp(argv[1], "client") == 0) {
        startClient();
    } else {
        std::cerr << "Invalid arg: " << argv[1] << std::endl;
        return 1;
    }
    return 0;
}