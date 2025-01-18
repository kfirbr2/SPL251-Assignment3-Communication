package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Connections;

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
    public static void handleConnect (String [] msg,int connectionId, Connections connection) {
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
}
