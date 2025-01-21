package bgu.spl.net.impl.stomp;

import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.ConnectionImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    private static AtomicInteger connectionId = new AtomicInteger(1);  

    public static int getAndIncrement() {
        return connectionId.getAndIncrement();  
    }
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Usage: StompServer <port> <serverType>");
            return;
        }

        int port = Integer.parseInt(args[0]);
        String serverType = args[1];

        Server<?> server;
        if ("tpc".equalsIgnoreCase(serverType)) {
            server = Server.threadPerClient(
                    port,
                    () -> new StompProtocol().start(getAndIncrement(), ConnectionImpl.getInstance()),
                    StompEncoderDecoder::new));
        } else if ("reactor".equalsIgnoreCase(serverType)) {
            server = Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port,
                    StompProtocol::new,
                    StompEncoderDecoder::new);
        } else {
            System.out.println("Invalid server type. Use 'tpc' or 'reactor'.");
            return;
        }

        server.serve();
    }

}
