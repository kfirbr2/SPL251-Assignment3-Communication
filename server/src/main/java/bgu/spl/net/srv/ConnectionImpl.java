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
    private final Map<Integer,Map<String, String>> subscriptions=new ConcurrentHashMap<>();// subid to channel

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
        //Remove from all channels
        for(String channel : subscriptions.get(connectionId).values()) {
            channels.get(channel).remove(connectionId);
            if (channels.get(channel).isEmpty()) {
                channels.remove(channel);
            }
        subscriptions.remove(connectionId);
        }
    }
    public void register(ConnectionHandler<T> handler, int connectionId){
        connections.put(connectionId, handler);
    }
    public void subscribe(String channel, int connectionId , String SubscriptionId){
        channels.computeIfAbsent(channel, k -> new CopyOnWriteArraySet<>()).add(connectionId);
        subscriptions.putIfAbsent(connectionId, new ConcurrentHashMap<>());
        subscriptions.get(connectionId).put(channel, SubscriptionId);
    }


    public void unsubscribe(int connectionId, String SubscriptionId){
        if(subscriptions.containsKey(connectionId)) {
            Map<String, String> userSubscriptions = subscriptions.get(connectionId);
            //find the correct channel
            String channelToRemove = null;
            for(Map.Entry<String, String> entry : userSubscriptions.entrySet()) {
                if(entry.getValue().equals(SubscriptionId)) {
                    channelToRemove = entry.getKey();
                    break;
                }
            }
            //Remove connectionId from the corrent channel
            if(channelToRemove != null) {
                userSubscriptions.remove(channelToRemove);
                if(channels.containsKey(channelToRemove)) {
                    channels.get(channelToRemove).remove(connectionId);
                }
                if(userSubscriptions.isEmpty()) {
                    subscriptions.remove(connectionId);
                }
            }
        }

    }


public  boolean isExistInChannel(String channel, int connectionId){
        if(channels.containsKey(channel)) {
            if(channels.get(channel).contains(connectionId)){
                return true;
            }
        }
return false;
    }

}
