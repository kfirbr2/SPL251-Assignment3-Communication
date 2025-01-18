package bgu.spl.net.impl.tpc;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class TpcProtocol implements StompMessagingProtocol<String> {
    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections connections;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId=connectionId;
        this.connections=connections;
    }


    @Override
   public void process(String msg){
        String[] parts = msg.split("\n");
        String command = parts[0];
        switch (command) {
            case "CONNECT":
                handleConnect(parts);
                break;
                case "DISCONNECT":
                    handleDisconnect(parts);
                    break;
                    case "SUBSCRIBE":
                        handleSubscribe(parts);
                        break;
                        case "UNSUBSCRIBE":
                            handleUnsubscribe(parts);
                            break;
                            case "SEND":
                                handleSend(parts);
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
