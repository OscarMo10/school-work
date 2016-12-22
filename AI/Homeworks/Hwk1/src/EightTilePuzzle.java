import java.util.*;

/**
 * Created by oscar on 9/9/16.
 */
public class EightTilePuzzle extends Puzzle {
    private final String mStartState;
    private final SearchHeuristic mHeuristic;

    public static final String GOAL_STATE = "012345678";


    public enum SearchHeuristic {
        MISPLACED_TILES,
        MANHATTAN_DISTANCE
    }

    public EightTilePuzzle(String startState, SearchHeuristic heuristic) {
        mStartState = startState;
        mHeuristic = heuristic;
    }

    @Override
    public String solve() {
        Node startNode = new Node(null, null, mStartState, 0);
        PriorityQueue<Node> fringe = new PriorityQueue<>(11, new Comparator<Node>() {
            @Override
            public int compare(Node node1, Node node2) {
                int gn1 = node1.getDepth();
                int gn2 = node2.getDepth();

                int hn1 = 0;
                int hn2 = 0;
                if (mHeuristic == SearchHeuristic.MANHATTAN_DISTANCE) {
                    hn1 = manhanttanDistance(node1.getState());
                    hn2 = manhanttanDistance(node2.getState());
                }
                else if (mHeuristic == SearchHeuristic.MISPLACED_TILES) {
                    hn1 = misplacedTiles(node1);
                    hn2 = misplacedTiles(node2);
                }


                if (gn1 + hn1 > gn2 + hn2) {
                    return 1;
                }
                else if (gn2 + hn2 > gn1 + hn1) {
                    return -1;
                }
                else {
                    return 0;
                }
            }
        });

        fringe.add(startNode);

        Node parent;
        while (!fringe.isEmpty()) {
            parent = fringe.poll();
            if (parent.getState().equals(GOAL_STATE)) return solution(parent);

            Node child;
            for (String action : possibleActions(parent)) {
                child = childNode(parent, action);
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


    private List<String> possibleActions(Node node) {
        int locationOfEmptyTile = node.getState().indexOf('0');

        ArrayList<String> possibleActions = new ArrayList<>();

        // 1st or 2nd row
        if (locationOfEmptyTile <= 5) {
            possibleActions.add(Action.UP);
        }

        // 3rd row
        if (locationOfEmptyTile > 2) {
            possibleActions.add(Action.DOWN);
        }

        // 1st or 2nd column
        if ( !(locationOfEmptyTile == 2 || locationOfEmptyTile == 5 || locationOfEmptyTile == 8) ) {
            possibleActions.add(Action.LEFT);
        }

        // 2nd or 3rd column
        if ( !(locationOfEmptyTile == 0 || locationOfEmptyTile == 3 || locationOfEmptyTile == 6) ) {
            possibleActions.add(Action.RIGHT);
        }
        return possibleActions;
    }

    private int misplacedTiles(Node node) {
        String nodeState = node.getState();

        int numOfMisplacedTiles = 0;

        for (int i = 0; i < nodeState.length(); ++i) {
            if (nodeState.charAt(i) != GOAL_STATE.charAt(i)) {
                numOfMisplacedTiles++;
            }
        }

        return numOfMisplacedTiles;
    }

    private int manhanttanDistance(String state) {
        int distance = 0;

        int goalRow;
        int goalColumn;
        int currRow;
        int currColumn;
        int indexOfTile;
        for (int i = 0; i < state.length(); ++i) {
            goalRow = i / 3;
            goalColumn = i % 3;

            indexOfTile = state.indexOf("" + i);
            currRow = indexOfTile / 3;
            currColumn = indexOfTile % 3;

            distance += Math.abs(goalRow - currRow) + Math.abs(goalColumn - currColumn);
        }

        return distance;
    }

}
