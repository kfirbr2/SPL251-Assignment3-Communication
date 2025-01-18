package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class Authenticator {
    private static final Map<String , String> users = new ConcurrentHashMap<>();
    private static final Map<String , Integer> activesUsers = new ConcurrentHashMap<>();

    public static synchronized String authenticate( String login, String passcode, int connectionId) {
        if(activesUsers.containsKey(login)) {
            return "ALREADY_LOGGED_IN";

        }
        if(users.containsKey(login) ) {
            if(users.get(login).equals(passcode)) {

                return "SUCCESS_EXISTING_USER";
            }
            else{
                return "WRONG_PASSCODE";
            }
        }else {
            activesUsers.put(login, connectionId);
            users.put(login, login);
            return "SUCCESS_NEW_USER";
        }
    }
    public static synchronized void logOut( String login) {
        activesUsers.remove(login);
    }
}
