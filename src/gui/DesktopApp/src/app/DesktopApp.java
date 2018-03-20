/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package app;

import java.io.File;
import java.util.LinkedList;
import java.util.List;
import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.Stage;

/**
 *
 * @author barcik
 */
public class DesktopApp extends Application {
   
    Stage window;
    Scene homeScene;

    @Override
    public void start(Stage primaryStage) {

        final Scene[] scenes;

        window = primaryStage;
        window.setTitle("Face Tweaking");

        // Layout Home Screen
        VBox homeLayout = new VBox(20);
        homeLayout.setPadding(new Insets(0, 25, 25, 25));

//       GridPane grid = new GridPane();
//       grid.setAlignment(Pos.CENTER);
//       grid.setHgap(10);
//       grid.setVgap(10);
//       grid.setPadding(new Insets(25, 25, 25, 25));
        Text scenetitle = new Text("Select the tweaking");
        scenetitle.setFont(Font.font("Tahoma", FontWeight.NORMAL, 20));
        homeLayout.getChildren().add(scenetitle);

//       grid.add(scenetitle, 0, 0, 2, 1);
        // Buttons for the features
        List<Button> featuresOptions = new LinkedList<>();
        File[] files = new File("./features").listFiles();
        for (File file : files) {
            featuresOptions.add(new Button(file.getName()));
        }

        scenes = new Scene[featuresOptions.size()];

        // Handling buttons
        // falta adicionar a chamada do .exe face_tweaking
        int counter = 0;
        for (Button b : featuresOptions) {
            VBox box = new VBox(20);
            box.setPadding(new Insets(15, 15, 15, 15));
            box.setAlignment(Pos.CENTER);
            Button returnBtn = new Button("return");
            returnBtn.setOnAction(e -> window.setScene(homeScene));
            Button runBtn = new Button("run");
            runBtn.setOnAction(e -> System.out.println("call program .exe"));
            Text text = new Text(b.getText());
            box.getChildren().add(text);
            
            // Get options from folder
            ObservableList<String> optionList = FXCollections.observableArrayList("a", "b", "c");
            ChoiceBox<String> options = new ChoiceBox<String>(optionList);
            box.getChildren().add(options);
            
            box.getChildren().add(runBtn);
            box.getChildren().add(returnBtn);
            
            Scene s = new Scene(box, 300, 250);
            scenes[counter] = s;
            b.setOnAction(e -> window.setScene(s));
            counter++;
        }

        homeLayout.getChildren().addAll(featuresOptions);
        homeLayout.setAlignment(Pos.CENTER);

        // Manual insertion of buttons
//       MenuButton beardButton = new MenuButton("Beard");
//       beardButton.getItems().addAll(new MenuItem("small"), new MenuItem("long"), new MenuItem("white"));
//       
//       MenuButton eyeButton = new MenuButton("Eyes");
//       eyeButton.getItems().addAll(new MenuItem("green"), new MenuItem("blue"), new MenuItem("brown"));
//             
//       MenuButton smileButton = new MenuButton("Smile");
//       smileButton.getItems().addAll(new MenuItem("female"), new MenuItem("male"));
//       
//       grid.add(beardButton, 0, 1);
//       grid.add(eyeButton, 0, 2);
//       grid.add(smileButton, 0, 3);


        Button exit = new Button("Exit");
        exit.setOnAction(e -> System.exit(0));
        HBox hbExit = new HBox(10);
        hbExit.setAlignment(Pos.BOTTOM_RIGHT);
        hbExit.getChildren().add(exit);
//       grid.add(hbExit, 1, 4);

        homeLayout.getChildren().add(hbExit);

        homeScene = new Scene(homeLayout, 300, 250);
//       Scene scene = new Scene(grid, 300, 275);
        window.setScene(homeScene);
        window.show();
        
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }

}
