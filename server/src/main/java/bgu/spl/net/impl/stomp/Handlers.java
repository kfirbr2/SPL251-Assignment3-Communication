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

    }
}
