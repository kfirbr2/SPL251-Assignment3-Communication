package bgu.spl.net.impl.tpc;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.Handlers;
import bgu.spl.net.srv.ConnectionImpl;
import bgu.spl.net.srv.Connections;

public class TpcProtocol implements StompMessagingProtocol<String> {
    private boolean shouldTerminate = false;
    private int connectionId;
    private ConnectionImpl connections;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId=connectionId;
        this.connections=(ConnectionImpl) connections;
    }


    @Override
   public void process(String msg){
        String[] parts = msg.split("\n");
        String command = parts[0];
        switch (command) {
            case "CONNECT":
                Handlers.handleConnect(parts, connectionId,connections);
                break;
            case "DISCONNECT":
                Handlers.handleDisconnect(parts,connectionId, connections);
                break;
            case "SUBSCRIBE":
                Handlers.handleSubscribe(parts,connectionId, connections);
                break;
            case "UNSUBSCRIBE":
                Handlers.handleUnsubscribe(parts,connectionId, connections);
                break;
            case "SEND":
                Handlers.handleSend(parts,connectionId, connections);
                break;
            default:
                sendError("Unknown command: " + command);

        }

    }
    @Override
    public boolean shouldTerminate(){
        return shouldTerminate();

    }
}
