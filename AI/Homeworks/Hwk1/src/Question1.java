import java.util.Scanner;

/**
 * Created by oscar on 9/13/16.
 */
public class Question1 {
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        System.out.print("Enter initial state: ");
        String initialState = in.next();

        // print goal state
        System.out.println("Goal State is " + FiveTilePuzzle.GOAL_STATE);

        // solve and print solution to puzzle
        FiveTilePuzzle puzzle = new FiveTilePuzzle(initialState);
        String solution = puzzle.solve();
        System.out.println("Solution: " + solution);
    }
}
