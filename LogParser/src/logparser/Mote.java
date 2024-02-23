/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author daniel
 */
public class Mote {
    private int id = 0;

    private Integer parentId = null;
    private ArrayList<MoteConversation> conversations = new ArrayList<MoteConversation>();
    private ArrayList<String> dmTexts = new ArrayList<String>();
    private ArrayList<String> ackTexts = new ArrayList<String>();

    public Mote(int id) {
        this.id = id;
    }
    
    public int getId() {
        return id;
    }

    public Integer getParentId() {
        return parentId;
    }

    public void setParentId(Integer parentId) {
        this.parentId = parentId;
    }

    /**
     * Extracts the parentID from the address
     * It accepts: "fe80::20e:e:e:e", "000e.000e.000e.000e" and simple 10-base strings as "12".
     * @param lladdr
     */
    public void setParentId(String lladdr) {
        if (lladdr.contains(".")) {
            String str = lladdr.substring(lladdr.lastIndexOf(".")+1);
            this.parentId = Integer.parseInt(str,16);
        } else if (lladdr.contains(":")) {
            String str = lladdr.substring(lladdr.lastIndexOf(":")+1);
            this.parentId = Integer.parseInt(str,16);
        } else {
            this.parentId = Integer.parseInt(lladdr,10);
        }
    }

    public void setId(int id) {
        this.id = id;
    }

    //---------------------------------------
    //data cache
    private BigDecimal dmTotal = null;
    private BigDecimal answeredDmTotal = null;

    private BigDecimal dmDelayTotal = null;
    private BigDecimal dmDelayAnwseredTotal = null;

    public BigDecimal getDmTotal() {
        return dmTotal;
    }

    public BigDecimal getAnsweredDmTotal() {
        return answeredDmTotal;
    }

    public BigDecimal getDmDelayTotal() {
        return dmDelayTotal;
    }

    public BigDecimal getDmDelayAnwseredTotal() {
        return dmDelayAnwseredTotal;
    }

    //---------------------------------------

    public ArrayList<MoteConversation> getConversations() {
        return conversations;
    }

    public void setConversations(ArrayList<MoteConversation> conversations) {
        this.conversations = conversations;
    }
    
    public void addConversation(MoteConversation mv) {
        this.conversations.add(mv);
        if (!this.dmTexts.contains(mv.getDmText())) {
            this.dmTexts.add(mv.getDmText());
        }
        if (!this.ackTexts.contains(mv.getAckText())) {
            this.ackTexts.add(mv.getAckText());
        }
    }


    public ArrayList<MoteConversation> getConversationsByDataMessage(String messageText) {
        ArrayList<MoteConversation> result = new ArrayList<MoteConversation>();
        
        for (MoteConversation mc : this.conversations) {
            if (mc.getDmText().equals(messageText)) {
                result.add(mc);
            }
        }
        return result;
    }

    public ArrayList<MoteConversation> getConversationsByAck(String ackText) {
        ArrayList<MoteConversation> result = new ArrayList<MoteConversation>();

        for (MoteConversation mc : this.conversations) {
            if (mc.getAckText() != null && mc.getAckText().equals(ackText)) {
                result.add(mc);
            }
        }
        return result;
    }

    public BigDecimal calculateTotalLoss() {
        int totalConversations = this.conversations.size();
        int answered = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived()) {
                answered++;
                if (mc.getAckText() != null && mc.getAckText().length() > 0) {
                    totalConversations++;
                    if (mc.isAckReceived()) {
                        answered++;
                    }
                }
            }
        }

        BigDecimal bgAnswered = new BigDecimal(answered);
        BigDecimal bgTotal = new BigDecimal(totalConversations);
        BigDecimal result = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 4, RoundingMode.HALF_UP))).multiply(new BigDecimal(100)).setScale(2);
        return result;
    }

    public HashMap<String,BigDecimal> calculateLossByDm() {
        HashMap<String, BigDecimal> result = new HashMap<String, BigDecimal>();
                        
        for (String msg : this.dmTexts) {
            ArrayList<MoteConversation> convByMessage = getConversationsByDataMessage(msg);
            int totalConversations = convByMessage.size();
            int answered = 0;
            for (MoteConversation mc : convByMessage) {
                if (mc.isDmReceived()) {
                    answered++;
                }
            }
            
            BigDecimal bgAnswered = new BigDecimal(answered);
            BigDecimal bgTotal = new BigDecimal(totalConversations);
            //BigDecimal value = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 2, RoundingMode.HALF_UP))).multiply(new BigDecimal(100));            
            BigDecimal value = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 4, RoundingMode.HALF_UP))).setScale(2);
            result.put(msg, value);
        }
        return result;
    }


    public BigDecimal calculateDmLoss() {
        int totalConversations = this.conversations.size();
        int received = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived()) {
                received++;
            }
        }

        //BigDecimal bgAnswered = new BigDecimal(received);
        this.answeredDmTotal = new BigDecimal(received);
        //BigDecimal bgTotal = new BigDecimal(totalConversations);
        this.dmTotal = new BigDecimal(totalConversations);

        BigDecimal result = (new BigDecimal(1).subtract(this.answeredDmTotal.divide(this.dmTotal, 4, RoundingMode.HALF_UP))).multiply(new BigDecimal(100)).setScale(2);
        return result;
    }


    public HashMap<String,BigDecimal> calculateLossByAck() {
        HashMap<String, BigDecimal> result = new HashMap<String, BigDecimal>();

        for (String msg : this.ackTexts) {
            ArrayList<MoteConversation> convByAck = getConversationsByAck(msg);
            int totalConversations = convByAck.size();
            int received = 0;
            for (MoteConversation mc : convByAck) {
                if (mc.isAckReceived()) {
                    received++;
                }
            }

            BigDecimal bgAnswered = new BigDecimal(received);
            BigDecimal bgTotal = new BigDecimal(totalConversations);
            //BigDecimal value = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 2, RoundingMode.HALF_UP))).multiply(new BigDecimal(100));
            BigDecimal value = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 4, RoundingMode.HALF_UP))).setScale(2);
            result.put(msg, value);
        }
        return result;
    }


    public BigDecimal calculateAckLoss() {
        //int totalConversations = this.conversations.size();
        int totalConversations = 0;
        int received = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived() && mc.getAckText() != null && mc.getAckText().length() > 0) {
                totalConversations++;
                if (mc.isAckReceived()) {
                    received++;
                }
            }
        }

        BigDecimal result = new BigDecimal(100);
        if (totalConversations > 0) {
            BigDecimal bgAnswered = new BigDecimal(received);
            BigDecimal bgTotal = new BigDecimal(totalConversations);
            result = (new BigDecimal(1).subtract(bgAnswered.divide(bgTotal, 4, RoundingMode.HALF_UP))).multiply(new BigDecimal(100)).setScale(2);
        }
        return result;
    }


    public BigDecimal calculateDmAverageDelay() {
        int totalDelay = 0;
        int answered = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived()) {
                answered++;
                totalDelay += mc.calculateDmDelay();
            }
        }
        this.dmDelayTotal = new BigDecimal(totalDelay);
        this.dmDelayAnwseredTotal = new BigDecimal(answered);
        BigDecimal result = null;
        if (answered > 0) {
            //result = new BigDecimal(totalDelay).divide(new BigDecimal(answered), 0, RoundingMode.HALF_UP);
            result = this.dmDelayTotal.divide(this.dmDelayAnwseredTotal, 0, RoundingMode.HALF_UP);
        }
        return result;
    }

    public BigDecimal calculateAckAverageDelay() {
        int totalDelay = 0;
        int answered = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived() && mc.isAckReceived()) {
                answered++;
                totalDelay += mc.calculateAckDelay();
            }
        }
        BigDecimal result = null;
        if (answered > 0) {
            result = new BigDecimal(totalDelay).divide(new BigDecimal(answered), 0, RoundingMode.HALF_UP);
        }
        return result;
    }


    public BigDecimal calculateTotalAverageDelay() {
        int totalDelay = 0;
        int answered = 0;
        for (MoteConversation mc : this.conversations) {
            if (mc.isDmReceived()) {
                answered++;
                totalDelay += mc.calculateDmDelay();
                if (mc.isAckReceived()) {
                    answered++;
                    totalDelay += mc.calculateAckDelay();
                }
            }
        }
        BigDecimal result = null;
        if (answered > 0) {
            result = new BigDecimal(totalDelay).divide(new BigDecimal(answered), 0, RoundingMode.HALF_UP);
        }
        return result;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Conversas do Mote ");
        sb.append(id);
        sb.append("\n");
        for (MoteConversation mc : this.conversations) {
            sb.append("\t");
            sb.append(mc.toString());            
            sb.append("\n");
        }   
        sb.append("PERDA MENSAGENS DE DADOS: ");
        sb.append(this.calculateDmLoss());
        sb.append(" %");
        sb.append("\n");
        sb.append("PERDA ACKS: ");
        sb.append(this.calculateAckLoss());
        sb.append(" %");
        sb.append("\n");
        sb.append("PERDA TOTAL: ");
        sb.append(this.calculateTotalLoss());
        sb.append(" %");
        sb.append("\n");
        sb.append("DELAY MÉDIO DE MENSAGENS DE DADOS: ");
        BigDecimal avgDmDelay = this.calculateDmAverageDelay();
        sb.append(avgDmDelay == null ? "-----------" : avgDmDelay.toString()+" ms");
        sb.append("\n");
        sb.append("DELAY MÉDIO DE ACKS: ");
        BigDecimal avgAckDelay = this.calculateAckAverageDelay();
        sb.append(avgAckDelay == null ? "-----------" : avgAckDelay.toString()+" ms");
        sb.append("\n");
        sb.append("DELAY MÉDIO TOTAL: ");
        BigDecimal avgTotalDelay = this.calculateTotalAverageDelay();
        sb.append(avgTotalDelay == null ? "-----------" : avgTotalDelay.toString()+" ms");
        sb.append("\n\n");
        return sb.toString();
    }
}
