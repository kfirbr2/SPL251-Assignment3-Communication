package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.impl.tpc.TpcProtocol;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        int port = 8080;//needed args[0]
        Server.threadPerClient(
                port, //port
                () -> new TpcProtocol(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
    }
}
