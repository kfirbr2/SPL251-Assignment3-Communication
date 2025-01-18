package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionImpl;

public class Handlers {
    private static String extractHeader (String [] lines , String key){
        for(String line : lines){
            if(line.startsWith(key + ":")){
                return line.substring((key + ":").length()).trim();

            }
        }
        return "";
    }
    private static String extractBody (String [] lines ){
        StringBuilder body = new StringBuilder();
        boolean bodyStarted=false;
        for(String line : lines){
         if(line.isEmpty()){
             bodyStarted=true;
             continue;
         }
         if(bodyStarted){
             body.append(line).append("\n");
         }
        }
        return body.toString().trim();

    }
    public static void handleConnect (String [] msg,int connectionId, ConnectionImpl connection) {
      String login=extractHeader(msg,"login");
      String passcode=extractHeader(msg,"passcode");
      String status=Authenticator.authenticate(login,passcode,connectionId);
      switch (status){
          case "ALREADY_LOGGED_IN":
              connection.send(connectionId,"ERROR\nmessage:The client is already logged in,log out before trying\n\n\u0000");
          break;
          case "SUCCESS_EXISTING_USER":
              connection.send(connectionId,"CONNECTED\nversion:1.2\n\n\u0000");
              break;
          case "WRONG_PASSCODE":
              connection.send(connectionId,"ERROR\nWrong password\n\n\u0000");
              break;
          case "SUCCESS_NEW_USER":
              connection.send(connectionId,"CONNECTED\nversion:1.2\n\n\u0000");
              break;

      }

    }
    public static void handleDisconnect (String [] msg,int connectionId, ConnectionImpl connection) {
        String receiptId=extractHeader(msg,"receipt");
        connection.send(connectionId,"RECEIPT\nreceipt-id:" + receiptId + "\n\n\u0000");
        Authenticator.logOut(connectionId);
        connection.disconnect(connectionId);
    }
    public static void handleSubscribe (String [] msg,int connectionId, ConnectionImpl connection) {
        String destination=extractHeader(msg,"destination");
        String id=extractHeader(msg,"id");
        if(destination.isEmpty() || id.isEmpty()){
            connection.send(connectionId,"ERROR\nmessage:Malformed SUBSCRIBE frame\n\n\u0000");
            connection.disconnect(connectionId);
            return;
        }
        connection.subscribe(destination,connectionId, id);
        connection.send(connectionId,"RECEIPT\nreceipt-id:" + id + "\n\n\u0000");

    }
    public static void handleUnsubscribe (String [] msg,int connectionId, ConnectionImpl connection) {
        String id=extractHeader(msg,"id");
        connection.unsubscribe(connectionId,id);
        connection.send(connectionId,"RECEIPT\nreceipt-id:" + id + "\n\n\u0000");

    }
    public static void handleSend (String [] msg,int connectionId, ConnectionImpl connection) {
        String destination=extractHeader(msg,"destination");
        if(connection.isExistInChannel(destination,connectionId)){
            connection.send(destination,extractBody(msg));
        }
        else {
            connection.send(connectionId,"ERROR\nmessage:Not aloud to send message in unsbscribed channel\n\n\u0000");
        }
    }

}
