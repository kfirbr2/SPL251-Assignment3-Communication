package bgu.spl.net.srv;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.*;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Supplier;

public abstract class BaseServer<T> implements Server<T> {

    private final int port;
    private final Supplier<StompMessagingProtocol<T>> protocolFactory;
    private final Supplier<StompMessageEncoderDecoder<T>> encdecFactory;
    private ServerSocket sock;
    private final AtomicInteger connectionId = new AtomicInteger(1);
    public BaseServer(
            int port,
            Supplier<StompMessagingProtocol<T>> protocolFactory,
            Supplier<StompMessageEncoderDecoder<T>> encdecFactory) {

        this.port = port;
        this.protocolFactory = protocolFactory;
        this.encdecFactory = encdecFactory;
		this.sock = null;
        
    }
      
   

    @Override
    public void serve() {

        try (ServerSocket serverSock = new ServerSocket(port)) {
			System.out.println("Server started ");

            this.sock = serverSock; //just to be able to close

            while (!Thread.currentThread().isInterrupted()) {

                Socket clientSock = serverSock.accept();
                int currentId = connectionId.getAndIncrement();
                BlockingConnectionHandler<T> handler = new BlockingConnectionHandler<>(
                        clientSock,
                        encdecFactory.get(),
                        protocolFactory.get(),currentId);
                        ((ConnectionImpl)(ConnectionImpl.getInstance())).register(handler,currentId);
                        


                execute(handler);
            }
        } catch (IOException ex) {
        }

        System.out.println("server closed!!!");
    }

    @Override
    public void close() throws IOException {
		if (sock != null)
			sock.close();
    }

    protected abstract void execute(BlockingConnectionHandler<T>  handler);

}
