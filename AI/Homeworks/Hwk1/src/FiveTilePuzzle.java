import java.util.*;

/**
 * Created by oscar on 9/3/16.
 */
public class FiveTilePuzzle extends Puzzle{
    private String mStartState;

    public static final String GOAL_STATE = "123450";


    public FiveTilePuzzle(String startState) {
        mStartState = startState;
    }


    // Solve puzzle using iterative depth search

    @Override
    public String solve() {
        // init starting node
        Node startNode = new Node(null, null, mStartState, 0);

        String result;
        for (int depth = 0; depth <= 25; ++depth) {
            result = iterativeDepthSearch(startNode, depth);
            if (result != null) {
                return result;
            }
        }

        return null;
    }

    private String iterativeDepthSearch(Node startNode, int limit) {
        if ( startNode.getState().equals(GOAL_STATE) ) {
            return solution(startNode);
        }

        Stack<Node> fringe = new Stack<>();
        fringe.add(startNode);

        Node node;
        Node child;
        while (!fringe.empty()) {
            node = fringe.pop();
            if(node.getDepth() == limit) continue;

            for (String action : possibleActions(node.getState())) {
                child = childNode(node, action);

                if (child.getState().equals(GOAL_STATE)) {
                    return solution(child);
                }
                else {
                    fringe.add(child);
                }

            }
        }


        return null;
    }


    // outputs list of actions possible at state
    private List<String> possibleActions(String state) {
        ArrayList<String> actionsPossible = new ArrayList<>();

        int indexOfEmptySpace = state.indexOf('0');

        // top row
        if (indexOfEmptySpace <= 2) {
            actionsPossible.add(Action.UP);
        }

        // bottom row
        if (indexOfEmptySpace >= 3) {
            actionsPossible.add(Action.DOWN);
        }

        // 1st column or 2nd column
        if (indexOfEmptySpace == 0 || indexOfEmptySpace == 3 || indexOfEmptySpace == 1 || indexOfEmptySpace == 4) {
            actionsPossible.add(Action.LEFT);
        }

        // 2nd column or 3rd column
        if (indexOfEmptySpace == 1 || indexOfEmptySpace == 4 || indexOfEmptySpace == 2 || indexOfEmptySpace == 5) {
            actionsPossible.add(Action.RIGHT);
        }

        return actionsPossible;
    }
}
