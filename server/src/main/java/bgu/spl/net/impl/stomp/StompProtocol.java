package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionImpl;
import bgu.spl.net.srv.Connections;

public class StompProtocol<T> implements StompMessagingProtocol<T> {


    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<T> connections;
    private Handlers handlers;

    @Override
    public void start(int connectionId, Connections<T> connections) {
        this.connectionId = connectionId;
        this.connections =  connections;
        this.handlers = new Handlers(this);
    }


    @Override
    public void process(T msg) {
        String[] parts = ((String)msg).split("\n");
        String command = parts[0];
        switch (command) {
            case "CONNECT":
                Handlers.handleConnect(parts, connectionId, connections);
                break;
            case "DISCONNECT":
                Handlers.handleDisconnect(parts, connectionId, connections, this);
                break;
            case "SUBSCRIBE":
                Handlers.handleSubscribe(parts, connectionId, connections);
                break;
            case "UNSUBSCRIBE":
                Handlers.handleUnsubscribe(parts, connectionId, connections);
                break;
            case "SEND":
                Handlers.handleSend(parts, connectionId, connections);
                break;
            default:
                Handlers.handleError("wrong headframe",connectionId,connections,null, this);

        }

    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate();

    }

    public void setshouldTerminate(boolean shouldTerminate) {
        this.shouldTerminate=shouldTerminate;
    }
}


