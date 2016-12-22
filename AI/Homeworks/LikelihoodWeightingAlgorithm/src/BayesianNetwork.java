import java.util.HashMap;
import java.util.List;
import java.util.Set;

/**
 * Created by oscar on 10/10/16.
 */
public class BayesianNetwork {
    private final List<Node> mNodes;
    private final Set<Pair<String, String>> mEdges;

    public BayesianNetwork(List<Node> nodes, Set<Pair<String,String>> edges) {
        mNodes = nodes;
        mEdges = edges;
    }

    public static Node createNode(String nodeName, Node[] parents, double[] distribution) {
        Node node = new BayesianNetwork.Node();
        node.nodeName = nodeName;
        node.parents = parents;
        node.distribution = distribution;

        return node;
    }

    public Node getNode(String nodeName) {
        for (Node node : mNodes) {
            if (node.nodeName.equals(nodeName)) {
                return node;
            }
        }

        return null;
    }

    public List<Node> getNodes() {
        return mNodes;
    }

    public Set<Pair<String, String>> getEdges() {
        return mEdges;
    }

    public static class Node {
        private String nodeName;
        private Node[] parents;
        private double[] distribution;

        public double getProbabilityEntry(boolean query, HashMap<String, Boolean> parents) {
            // return prior probablity if node has no parent
            if (this.parents == null) {
                if (query) {
                    return distribution[1];
                }
                else {
                    return 1 - distribution[1];
                }
            }

            int index = 0;
            for (int i = 0; i < this.parents.length; i++) {
                String parentName = this.parents[this.parents.length - i - 1].nodeName;

                int parentValue = parents.get(parentName) ? 1 : 0;
                index += parentValue * Math.pow(2, i);
            }

            if (query) {
                return distribution[index];
            }
            else {
                return 1 - distribution[index];
            }
        }

        public String getNodeName() {
            return nodeName;
        }
    }
}
