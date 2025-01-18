package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArraySet;

public class ConnectionImpl implements Connections<String> {
    private final Map<Integer, ConnectionHandler<String>> connections=new ConcurrentHashMap<>(); //
    private final Map<String, Set<Integer>> channels=new ConcurrentHashMap<>();// channel to ids
    private final Map<String,String> subId=new ConcurrentHashMap<>();// subid to channel

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
        for (Set<Integer> channel : channels.values()) {
            channel.remove(connectionId);
        }

    }
    public void register(ConnectionHandler<String> handler, int connectionId){
        connections.put(connectionId, handler);
    }
    public void subscribe(String channel, int connectionId , String SubscriptionId){
        channels.computeIfAbsent(channel, k -> new CopyOnWriteArraySet<>()).add(connectionId);
        subId.put(SubscriptionId, channel);
    }
    public void unsubscribe(int connectionId, String SubscriptionId){
        String channel= subId.get(SubscriptionId);
        Set<Integer> subscribers = channels.get(channel);
        if (subscribers != null) {
            subscribers.remove(connectionId);
            if(subscribers.isEmpty()) {
                channels.remove(channel);
            }
        }

    }
    public  boolean isExistInChannel(String channel, int connectionId){
        return channels.get(channel).contains(connectionId);

    }

}
