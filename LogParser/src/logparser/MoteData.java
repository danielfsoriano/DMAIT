/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author daniel
 */
public class MoteData {
    
    private HashMap<Integer, Mote> moteList = new HashMap<Integer, Mote>();
    
    public HashMap<Integer, Mote> getMoteList() {
        return moteList;
    }

    public ArrayList<Integer> attackers = new ArrayList<>();

    public void setMoteList(HashMap<Integer, Mote> moteList) {
        this.moteList = moteList;
    }



    public void addMoteConversation(int moteId, MoteConversation mc) {
        Mote mote = null;
        if (this.moteList.containsKey(moteId)) {
            mote = this.moteList.get(moteId);            
        } else {
            mote = new Mote(moteId);            
        }
        mote.addConversation(mc);
        this.moteList.put(moteId, mote);
    }

    public void setMoteParent(int moteId, String parent) {
        Mote mote = null;
        if (this.moteList.containsKey(moteId)) {
            mote = this.moteList.get(moteId);
        } else {
            mote = new Mote(moteId);
        }
        mote.setParentId(parent);
        this.moteList.put(moteId, mote);
    }

    public void addAtacker(Integer nodeId) {
        if (!this.attackers.contains(nodeId)) {
            this.attackers.add(nodeId);
        }
    }

    public ArrayList<Integer> getAttackers() {
        return this.attackers;
    }
}
