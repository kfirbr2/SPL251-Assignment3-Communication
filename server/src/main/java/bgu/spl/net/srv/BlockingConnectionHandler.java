package bgu.spl.net.srv;
import bgu.spl.net.impl.stomp.*;
import bgu.spl.net.api.StompMessagingProtocol;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocol<T> protocol;
    private final StompMessageEncoderDecoder<T> encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;

    public BlockingConnectionHandler(Socket sock, StompMessageEncoderDecoder<T> reader, StompMessagingProtocol<T> protocol , int connectionId) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        this.protocol.start(connectionId,(ConnectionImpl)ConnectionImpl.getInstance());
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                T nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                     protocol.process(nextMessage);
                     
                    }
                }
           

        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        try {
            byte[] encodedMsg = encdec.encode(msg); // Convert message to bytes
            out.write(encodedMsg); // Send bytes to client
            out.flush(); // Ensure all data is sent
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
