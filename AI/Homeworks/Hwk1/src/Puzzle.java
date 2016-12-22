import java.util.ArrayList;
import java.util.Collections;

/**
 * Created by oscar on 9/13/16.
 */
public abstract class Puzzle {

    public class Node {
        private final Node mParent;
        private final String mAction;
        private final String mState;
        private final int mDepth;

        public Node(Node parent, String action, String state, int depth) {
            mParent = parent;
            mAction = action;
            mState = state;
            mDepth = depth;
        }

        public Node getParent() {
            return mParent;
        }

        public String getAction() {
            return mAction;
        }

        public String getState() {
            return mState;
        }

        public int getDepth() {
            return mDepth;
        }
    }

    public class Action {
        public static final String LEFT = "L";
        public static final String RIGHT = "R";
        public static final String UP = "U";
        public static final String DOWN = "D";
    }

    protected String solution(Node node) {
        // if start state was goal state
        if (node.getParent() == null) {
            return "";
        }

        ArrayList<String> result = new ArrayList<>();
        Node child = node;
        for (int i = 0; i < node.getDepth(); ++i) {
            result.add(child.getAction());
            child = child.getParent();
        }

        Collections.reverse(result);
        return String.join("-", result);
    }

    protected Node childNode(Node parent, String action) {
        final int emptySpaceIndex = parent.getState().indexOf('0');

        int newIndexOfEmptySpace;
        switch (action) {
            case Action.LEFT:
                newIndexOfEmptySpace = emptySpaceIndex + 1;
                break;
            case Action.RIGHT:
                newIndexOfEmptySpace = emptySpaceIndex - 1;
                break;
            case Action.UP:
                newIndexOfEmptySpace = emptySpaceIndex + 3;
                break;
            case Action.DOWN:
                newIndexOfEmptySpace = emptySpaceIndex - 3;
                break;
            default:
                throw new RuntimeException("Invalid action");
        }

        // switch empty space and neighbor
        char temp;
        char[] newState = parent.getState().toCharArray();
        temp = newState[newIndexOfEmptySpace];
        newState[newIndexOfEmptySpace] = '0';
        newState[emptySpaceIndex] = temp;

        return new Node(parent, action, new String(newState), parent.getDepth() + 1);

    }

    public abstract String solve();
}
