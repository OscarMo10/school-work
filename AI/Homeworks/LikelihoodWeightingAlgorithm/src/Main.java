
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Main {
    private static final String queryExpression = "p\\((?<queryVar>(performance|service|teaching|education|research) = (high|low)) [|] (?<evidence>(performance|teaching|service|education|research) = (high|low)( and (performance|teaching|service|education|research) = (high|low))*)\\)";
    private static final String queryVariableValueExpression = "(?<queryVariableName>performance|teaching|service|education|research) = (?<queryVariableValue>high|low)";

    public static void main(String[] args) throws Exception {
        BayesianNetwork bn = createNetwork();
        String[] questionQueries = new String[] {
                "p(performance = high | education = high)",
                "p(service = high | teaching = low and performance = high)",
                "p(education = low | teaching = high and performance = low)"
        };


        System.out.println("Queries for part c:");
        for (String query : questionQueries) {
            double probability = answerQuery(bn, query);
            String probString = String.format("%.3f", probability);
            System.out.println(query + " = " + probString);
        }
        System.out.println();

        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print("Enter query: ");
            String query = scanner.nextLine();
            double probability;
            try {
                probability = answerQuery(bn, query);
            }
            catch (Exception e) {
                System.out.println("Invalid query supplied. Refer to README for proper query formation.");
                continue;
            }
            String probString = String.format("%.3f", probability);
            System.out.println(query + " = " + probString);
        }

    }

    private static double answerQuery(BayesianNetwork bn, String query) throws Exception {
        Pattern queryPattern = Pattern.compile(queryExpression);
        Matcher matcher;// if doesnt match query format
        matcher = queryPattern.matcher(query);
        if (!matcher.matches()) {
            System.out.println("Query format did not match that specified in README file.");
            throw new Exception("Invalid query format supplied to answerQuery");
        }
        HashMap<String, Boolean> evidence = getEvidence(query);
        Pair<String, Boolean> queryVar = getQueryVariable(query);
        double[] distribution = LikelihoodWeightingAlgorithm.LikelihoodWeighting(queryVar.getKey(), evidence, bn, 100);

        if (queryVar.getValue()) {
            return distribution[1];
        }
        else {
            return distribution[0];
        }
    }

    private static BayesianNetwork createNetwork() {
        // create Node education
        BayesianNetwork.Node nodeE = BayesianNetwork.createNode("education", null, new double[]{ 0.0, 0.7});

        // Create Node teaching
        BayesianNetwork.Node[] parentsT = new BayesianNetwork.Node[] { nodeE };
        double[] distributionT = new double[] { 0.15, 0.45 };
        BayesianNetwork.Node nodeT = BayesianNetwork.createNode("teaching", parentsT, distributionT);

        // Create Node research
        BayesianNetwork.Node[] parentsR = new BayesianNetwork.Node[] { nodeE };
        double[] distributionR = new double[] { 0.35, 0.85 };
        BayesianNetwork.Node nodeR= BayesianNetwork.createNode("research", parentsR, new double[]{0.0, 0.7});

        //Create Node service
        double[] distributionS = new double[] { 0.0, 0.40 };
        BayesianNetwork.Node nodeS = BayesianNetwork.createNode("service", null, distributionS);

        // Create Node P
        BayesianNetwork.Node[] parentsP = new BayesianNetwork.Node[] { nodeT, nodeR, nodeS};
        double[] distributionP = new double[] { 0.05, 0.05, 0.5, 0.5, 0.3, 0.3, 0.8, 0.95 };
        BayesianNetwork.Node nodeP = BayesianNetwork.createNode("performance", parentsP, distributionP);

        // Create Edges
        HashSet<Pair<String, String>> edges = new HashSet<Pair<String, String>>();
        edges.add(new Pair<String, String>("E", "T"));
        edges.add(new Pair<String, String>("E", "R"));
        edges.add(new Pair<String, String>("", "D"));

        // Create Bayesian Network
        ArrayList<BayesianNetwork.Node> nodes = new ArrayList<BayesianNetwork.Node>();
        nodes.add(nodeE);
        nodes.add(nodeT);
        nodes.add(nodeR);
        nodes.add(nodeS);
        nodes.add(nodeP);

        BayesianNetwork network = new BayesianNetwork(nodes, edges);
        return network;
    }

    private static Pair<String, Boolean> getQueryVariable(String query) {
        final HashMap<String, Boolean> valMap = new HashMap<>();
        valMap.put("high", true);
        valMap.put("low", false);

        Pattern queryPattern = Pattern.compile(queryExpression);
        Matcher queryMatcher = queryPattern.matcher(query);
        queryMatcher.find();

        String queryVarStr = queryMatcher.group("queryVar");
        Pattern queryVarValPattern = Pattern.compile(queryVariableValueExpression);
        Matcher matcher = queryVarValPattern.matcher(queryVarStr);
        matcher.find();
        String value = matcher.group("queryVariableValue");
        return new Pair<>(matcher.group("queryVariableName"), valMap.get(value));
    }

    public static HashMap<String, Boolean> getEvidence(String query) {
        final HashMap<String, Boolean> valMap = new HashMap<>();
        valMap.put("high", true);
        valMap.put("low", false);

        Pattern queryPattern = Pattern.compile(queryExpression);
        Matcher queryMatcher = queryPattern.matcher(query);
        queryMatcher.find();

        String evidenceString = queryMatcher.group("evidence");
        Pattern queryVarValPattern = Pattern.compile(queryVariableValueExpression);
        Matcher matcher = queryVarValPattern.matcher(evidenceString);
        HashMap<String, Boolean> evidence = new HashMap<>();
        while( matcher.find() ) {
            String varName = matcher.group("queryVariableName");
            boolean value = valMap.get(matcher.group("queryVariableValue"));
            evidence.put(varName, value);
        }
        return evidence;
    }
}
