package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArraySet;

public class ConnectionImpl<T> implements Connections<T> {
    private final Map<Integer, ConnectionHandler<T>> connections=new ConcurrentHashMap<>(); //
    private final Map<String, Set<Integer>> channels=new ConcurrentHashMap<>();// channel to ids
    private final Map<Integer,String> subscriptions=new ConcurrentHashMap<>();// subid to channel

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler =connections.get(connectionId);
        if (handler != null) {
            handler.send(msg);
            return true;
        }
     return false;
    }
    @Override
    public void send(String channel, T msg){
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
        subscriptions.remove(connectionId);
        for (Set<Integer> channel : channels.values()) {
            if(channel != null && channel.contains(connectionId)) {
                channel.remove(connectionId);
            }
        }
    }
    public void register(ConnectionHandler<T> handler, int connectionId){
        connections.put(connectionId, handler);
    }
    public void subscribe(String channel, int connectionId , String SubscriptionId){
        channels.computeIfAbsent(channel, k -> new CopyOnWriteArraySet<>()).add(connectionId);
        subscriptions.put(connectionId, SubscriptionId);
    }
    public void unsubscribe(int connectionId, String SubscriptionId){
        subscriptions.entrySet().removeIf(entry -> entry.getKey().equals(connectionId) && entry.getValue().equals(SubscriptionId));
        channels.forEach((k, v) -> v.remove(connectionId));
        }
}
