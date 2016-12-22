import java.util.HashMap;
import java.util.Random;

/**
 * Created by oscar on 10/10/16.
 */
public class LikelihoodWeightingAlgorithm {
    public static double[] LikelihoodWeighting(String X, HashMap<String, Boolean> e,
                                               BayesianNetwork bayesianNetwork, int N) {
        double[] W = new double[2];
        for (int j = 1; j <= N; j++) {
            Pair<HashMap<String,Boolean>, Double> sampleAndWeight = weightedSample(bayesianNetwork, e);
            HashMap<String, Boolean> sample = sampleAndWeight.getKey();
            double w = sampleAndWeight.getValue();

            if (sampleAndWeight.getKey().get(X) == false) {
                W[0] += w;
            }
            else {
                W[1] += w;
            }
        }

        return normalize(W);
    }

    public static double[] normalize(double[] W) {
        W[0] = W[0] / (W[0] + W[1]);
        W[1] = 1 - W[0];
        return W;
    }

    public static Pair<HashMap<String, Boolean>, Double> weightedSample(BayesianNetwork network, HashMap<String, Boolean> e) {
        double w = 1;
        boolean[] X = new boolean[network.getNodes().size()];
        HashMap<String, Boolean> sample = new HashMap<>();
        sample.putAll(e);

        for (BayesianNetwork.Node node : network.getNodes()) {
            // if node is evidence variable in e
            if (e.containsKey(node.getNodeName())) {
                boolean variableValue = e.get(node.getNodeName());
                w = w * node.getProbabilityEntry(variableValue, sample);
            }
            else {
                double truthProbability = node.getProbabilityEntry(true, sample);

                Random random = new Random();
                double randomVal = random.nextDouble();

                boolean sampleValue;
                if (randomVal < truthProbability) {
                    sampleValue = true;
                }
                else {
                    sampleValue = false;
                }

                sample.put(node.getNodeName(), sampleValue);
            }
        }

        return new Pair<>(sample, w);
    }
}
