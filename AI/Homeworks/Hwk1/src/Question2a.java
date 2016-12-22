import java.util.Scanner;

/**
 * Created by oscar on 9/13/16.
 */
public class Question2a {
    public static void main(String[] args) {
        System.out.println("Enter initial state: ");
        Scanner in = new Scanner(System.in);
        String initialState = in.next();

        // print goal state
        System.out.println("Goal State is " + EightTilePuzzle.GOAL_STATE);

        // solve and print solution
        EightTilePuzzle puzzle = new EightTilePuzzle(initialState, EightTilePuzzle.SearchHeuristic.MISPLACED_TILES);
        String solution = puzzle.solve();
        System.out.println("Solution: " + solution);

    }
}
