package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArraySet;

public class ConnectionImpl implements Connections<String> {
    private final Map<Integer, ConnectionHandler<String>> connections=new ConcurrentHashMap<>();
    private final Map<String, Set<Integer>> channels=new ConcurrentHashMap<>();

    @Override
    public boolean send(int connectionId, String msg) {
        ConnectionHandler<String> handler =connections.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
     return false;
    }
    @Override
    public void send(String channel, String msg){
        Set<Integer> subscribers =channels.get(channel);
        if (subscribers != null) {
            for(Integer subscriber : subscribers) {
                send(subscriber, msg);
            }

        }


    }
    @Override
    public void disconnect(int connectionId){
        connections.remove(connectionId);
        channels.values().forEach(subscribers -> subscribers.remove(connectionId));

    }
    public void register(ConnectionHandler<String> handler, int connectionId){
        connections.put(connectionId, handler);
    }
    public void subscribe(String channel, int connectionId){
        channels.computeIfAbsent(channel, k -> new CopyOnWriteArraySet<>()).add(connectionId);
    }
    public void unsubscribe(String channel, int connectionId){
        Set<Integer> subscribers = channels.get(channel);
        if (subscribers != null) {
            subscribers.remove(connectionId);
            if(subscribers.isEmpty()) {
                channels.remove(channel);
            }
        }

    }

}
