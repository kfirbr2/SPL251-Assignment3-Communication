package bgu.spl.net.impl.tpc;

import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class TpcServer {
    public static void main(String[] args) {
        Server.threadPerClient(
                7777, //port
                () -> new EchoProtocol(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();

    }
}
