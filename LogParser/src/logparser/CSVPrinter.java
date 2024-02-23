/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package logparser;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.*;

/**
 *
 * @author daniel
 */
public class CSVPrinter {

    private MoteData moteData = null;
    
    public CSVPrinter(MoteData moteData) {
        setMoteData(moteData);  
    }
            
    private void setMoteData(MoteData moteData) {
        this.moteData = moteData;
    }

    public MoteData getMoteData() {
        return moteData;
    }
    
    public void printDelayCSV() {        
        System.out.println("IMPRIMINDO TABELA DE DELAYS");
        if (this.moteData != null) {

            HashMap<Integer, Mote> motes = this.moteData.getMoteList();
            
            //ordenando os ids
            TreeSet<Integer> idSet = new TreeSet<Integer>();
            idSet.addAll(motes.keySet());
            ArrayList<Integer> moteIds = new ArrayList<Integer>(idSet);

            ArrayList<Long> delayList = new ArrayList<>();

            Long highestDelay = 0L;
            Long lowestDelay = Long.MAX_VALUE;

            AverageCalculator totalAvgDelayCalculator = new AverageCalculator();

            for (Integer key : moteIds) {
                Mote mote = motes.get(key);

                HashMap<String, AverageCalculator> averageDelayByMessageByMote = new HashMap<String, AverageCalculator>();
          
                for (MoteConversation mc : mote.getConversations()) {
                    AverageCalculator ac = averageDelayByMessageByMote.get(mc.getDmText());
                    if (ac == null) {
                        ac = new AverageCalculator();
                    }

                    Long delay = mc.calculateDmDelay();

                    if (delay != null) {
                        ac.addValue(delay);
                        totalAvgDelayCalculator.addValue(delay);

                        //highest and lowest
                        if (delay > highestDelay) highestDelay = delay;
                        if (delay < lowestDelay) lowestDelay = delay;

                        delayList.add(delay);
                    }
                    
                    averageDelayByMessageByMote.put(mc.getDmText(), ac);
                }                
                printMoteDelayLine(mote, averageDelayByMessageByMote);                
            }

            BigDecimal avgDelay = totalAvgDelayCalculator.calculateAverage();
            System.out.println("DELAY MINIMO DE MSGS DE DADOS DE TODOS OS MOTES: " + lowestDelay.toString().replace(".", ",") + " ms");
            System.out.println("DELAY MAXIMO DE MSGS DE DADOS DE TODOS OS MOTES: " + highestDelay.toString().replace(".", ",") + " ms");
            System.out.println("DELAY MEDIO DE MSGS DE DADOS DE TODOS OS MOTES: " + avgDelay.toString().replace(".", ",") + " ms");
            System.out.println("DESVIO PADRÃO DO DELAY: " + calculateStandardDeviation(delayList, avgDelay.doubleValue()).toString().replace(".", ","));


        } else {
            System.out.println("Sem moteData!");
        }
    }

    public void printRTTDelayCSV() {
        System.out.println("IMPRIMINDO TABELA DE DELAYS RTT");
        if (this.moteData != null) {

            HashMap<Integer, Mote> motes = this.moteData.getMoteList();

            //ordenando os ids
            TreeSet<Integer> idSet = new TreeSet<Integer>();
            idSet.addAll(motes.keySet());
            ArrayList<Integer> moteIds = new ArrayList<Integer>(idSet);

            ArrayList<Long> rttDelayList = new ArrayList<>();

            Long highestRTTDelay = 0L;
            Long lowestRTTDelay = Long.MAX_VALUE;

            AverageCalculator totalAvgRTTDelayCalculator = new AverageCalculator();

            for (Integer key : moteIds) {
                Mote mote = motes.get(key);

                HashMap<String, AverageCalculator> averageRTTDelayByMessageByMote = new HashMap<String, AverageCalculator>();

                for (MoteConversation mc : mote.getConversations()) {
                    AverageCalculator aRTTc = averageRTTDelayByMessageByMote.get(mc.getDmText());
                    if (aRTTc == null) {
                        aRTTc = new AverageCalculator();
                    }

                    Long delay = mc.calculateDmDelay();
                    Long ackDelay = mc.calculateAckDelay();

                    if (delay != null && ackDelay != null) {
                        Long rttDelay = delay + ackDelay;
                        aRTTc.addValue(rttDelay);

                        totalAvgRTTDelayCalculator.addValue(rttDelay);
                        //highest and lowest
                        if (rttDelay > highestRTTDelay) highestRTTDelay = rttDelay;
                        if (rttDelay < lowestRTTDelay) lowestRTTDelay = rttDelay;

                        rttDelayList.add(rttDelay);
                    }

                    averageRTTDelayByMessageByMote.put(mc.getDmText(), aRTTc);
                }
                printMoteDelayLine(mote, averageRTTDelayByMessageByMote);
            }

            BigDecimal avgRTTDelay = totalAvgRTTDelayCalculator.calculateAverage();
            if (avgRTTDelay != null) {
                System.out.println("DELAY MINIMO RTT DE MSGS DE DADOS DE TODOS OS MOTES: " + lowestRTTDelay.toString().replace(".", ",") + " ms");
                System.out.println("DELAY MAXIMO RTT DE MSGS DE DADOS DE TODOS OS MOTES: " + highestRTTDelay.toString().replace(".", ",") + " ms");
                System.out.println("DELAY MEDIO RTT DE MSGS DE DADOS DE TODOS OS MOTES: " + avgRTTDelay.toString().replace(".", ",") + " ms");
                System.out.println("DESVIO PADRÃO DO RTT DELAY: " + calculateStandardDeviation(rttDelayList, avgRTTDelay.doubleValue()).toString().replace(".", ","));
            } else {
                System.out.println("NAO FOI POSSIVEL CALCULAR MAIS DADOS RTT POIS avgRTTDelay VEIO NULO - PROVAVELMENTE O ENVIO DE ACKS ESTAVA DESATIVADO");
            }

        } else {
            System.out.println("Sem moteData!");
        }
    }


    public void printMoteParents() {
        System.out.println("IMPRIMINDO PAIS DOS NOS");
        if (this.moteData != null) {

            HashMap<Integer, Mote> motes = this.moteData.getMoteList();

            //ordenando os ids
            TreeSet<Integer> idSet = new TreeSet<Integer>();
            idSet.addAll(motes.keySet());
            ArrayList<Integer> moteIds = new ArrayList<Integer>(idSet);


            HashMap<Integer, ArrayList<Integer>> parentMap = new HashMap<>();

            for (Integer key : moteIds) {
                Mote mote = motes.get(key);
                System.out.println("NO " + mote.getId() + ", PAI: " + mote.getParentId());

                //build the tree
                ArrayList<Integer> children = parentMap.get(mote.getParentId());
                if (children == null) {
                    children = new ArrayList<>();
                }
                children.add(mote.getId());
                parentMap.put(mote.getParentId(), children);
            }
            System.out.println("\n");

            for (Integer key : parentMap.keySet()) {
                System.out.print("PAI: " + key + ", FILHOS: ");
                for (int son : parentMap.get(key)) {
                    System.out.print(son + ", ");
                }
                System.out.println();
            }

            System.out.println();
            System.out.println();

            ArrayList<Integer> atacantes = moteData.getAttackers();
            HashMap<Integer, Object> globalAffectedNodes = new HashMap<>();
            System.out.println("NOS AFETADOS PELO ATAQUE (ATACANTES: " + Arrays.toString(atacantes.toArray()) + ")");
            for (Integer attacker : atacantes) {
                LinkedList<Integer> sons = new LinkedList<>();
                ArrayList<Integer> affectedNodes = new ArrayList<>();
                if (parentMap.containsKey(attacker)) {
                    sons.addAll(parentMap.get(attacker));
                }
                while (sons.peek() != null) {
                    Integer son = sons.poll();
                    affectedNodes.add(son);
                    globalAffectedNodes.put(son, null);

                    if (parentMap.containsKey(son)) {
                        sons.addAll(parentMap.get(son));
                    }
                }
                System.out.println("ATACANTE: " + attacker + " AFETOU : " + affectedNodes.toString());

            }
            System.out.println("TOTAL DE NOS AFETADOS: " + globalAffectedNodes.keySet().size());
            System.out.println("OBS: CONSIDERANDO OS NOS ATACANTES COMO AFETADOS, SE FOREM FILHOS DE OUTROS ATACANTES");

        } else {
            System.out.println("Sem moteData!");
        }
    }


    private Double calculateStandardDeviation(ArrayList<Long> data, Double average) {
        //calc the sum of the differences
        Double diffSumSquared = 0.0;
        for (Long current : data) {
            diffSumSquared += Math.pow(((double) current) - ((double) average), 2);
        }
        Double result = Math.sqrt(diffSumSquared / data.size());
        return result;
    }

    private void printMoteDelayLine(Mote mote, HashMap<String, AverageCalculator> averageDelayByMessageByMote) {
        System.out.print(mote.getId());   
        ArrayList<String> messages = new ArrayList<String>();

        for (MoteConversation mc : mote.getConversations()) {
            if (!messages.contains(mc.getDmText())) {
                messages.add(mc.getDmText());
            }
        }
     
/*
        HashMap<Integer, String> indexedMessages = new HashMap<Integer, String>();
        for (MoteConversation mc : mote.getConversations()) {
            Integer number = Integer.valueOf(mc.getMessage().replaceAll("Hello ", "").trim());
            if (!indexedMessages.containsKey(number)) {
                indexedMessages.put(number, mc.getMessage());
            }
        }
     
        for (int x = 1; x <= indexedMessages.size(); x++) {
*/            
        
        for (String message : messages) {
            System.out.print(";");
            //System.out.print(message + " - ");
            BigDecimal average = averageDelayByMessageByMote.get(message).calculateAverage();
            System.out.print(average != null ? average : "----");
        }
        System.out.print("\n");        
    }

    
    public void printLossCSV() {        
        System.out.println("IMPRIMINDO TABELA DE PERDAS");
        if (this.moteData != null) {
            
            HashMap<Integer, Mote> motes = this.moteData.getMoteList();
            
            //ordenando os ids
            TreeSet<Integer> idSet = new TreeSet<Integer>();
            idSet.addAll(motes.keySet());
            ArrayList<Integer> moteIds = new ArrayList<Integer>(idSet);

            int allMotesConversations = 0;
            int allMotesLosses = 0;

            for (Integer key : moteIds) {
                Mote mote = motes.get(key);
                
                HashMap<String, BigDecimal> averageDmLossByMessageByMote = mote.calculateLossByDm();
                allMotesConversations = allMotesConversations + averageDmLossByMessageByMote.values().size();
                for (BigDecimal losses : averageDmLossByMessageByMote.values()) {
                    if (losses.floatValue() > 0) {
                        allMotesLosses++;
                    }
                }
          
                printMoteLossLine(mote, averageDmLossByMessageByMote);
            }

            BigDecimal totalLossPercentage = new BigDecimal(allMotesLosses).divide(new BigDecimal(allMotesConversations), 5, RoundingMode.HALF_UP).multiply(new BigDecimal(100));
            System.out.println("PERDA MEDIA DE MSGS DE DADOS DE TODOS OS MOTES: " + totalLossPercentage.toString().replace(".", ",") + "%");
        } else {
            System.out.println("Sem moteData!");
        }        
    }

    private void printMoteLossLine(Mote mote, HashMap<String, BigDecimal> averageLossByMessageByMote) {
        System.out.print(mote.getId());   
        ArrayList<String> messages = new ArrayList<String>();

        for (MoteConversation mc : mote.getConversations()) {
            if (!messages.contains(mc.getDmText())) {
                messages.add(mc.getDmText());
            }
        }
     
/*
        HashMap<Integer, String> indexedMessages = new HashMap<Integer, String>();
        for (MoteConversation mc : mote.getConversations()) {
            Integer number = Integer.valueOf(mc.getMessage().replaceAll("Hello ", "").trim());
            if (!indexedMessages.containsKey(number)) {
                indexedMessages.put(number, mc.getMessage());
            }
        }
     
        for (int x = 1; x <= indexedMessages.size(); x++) {
*/            
        
        for (String message : messages) {
            System.out.print(";");
            //System.out.print(message + " - ");
            BigDecimal average = averageLossByMessageByMote.get(message);
            System.out.print(average != null ? average.toString().replace(".", ",") : "----");
        }
        System.out.print("\n");        
    }
    
    
}
