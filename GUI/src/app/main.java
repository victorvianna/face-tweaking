/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package app;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
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
public class main extends Application {
   
    Stage window;
    Scene homeScene;
    
    private final String FEATURES_DIR = "../lib/features";
    private final String ENGINE_DIR = "../engine/cmake-build-debug/engine";
    private final String IMAGE_DIR = "../image.jpg";

    @Override
    public void start(Stage primaryStage) throws Exception {
        final int[] index = new int [1]; index[0]= 0;
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
        List<Button> features = new LinkedList<>();
        ArrayList<File> files= new ArrayList<>();
        ArrayList<File> featureFiles =new ArrayList<>();
        files.add(new File(FEATURES_DIR));
        for(int i=0;i<files.size(); i++){
            File file = files.get(i);
            if (file.getName().toLowerCase().endsWith(".cpp") && 
                    !file.getName().equals("BaseFeature.cpp")) {
                features.add(new Button(shortName(file)));
                featureFiles.add(file);
            }
            else if (file.isDirectory())
            {
                File[] sons = file.listFiles();
                for(File son : sons)
                    files.add(son);
            }
        }
        
        scenes = new Scene[features.size()];

        
        ArrayList<ArrayList<File> > featOptions = new ArrayList<>();
        
        // Handling buttons
        
        int counter = 0;
        for (int i=0; i<features.size(); i++) {
            featOptions.add(new ArrayList<>());
            Button b = features.get(i);
            File feature = featureFiles.get(i);
            ObservableList<String> optionList = FXCollections.observableArrayList();
            VBox box = new VBox(20);
            box.setPadding(new Insets(15, 15, 15, 15));
            box.setAlignment(Pos.CENTER);
            Button returnBtn = new Button("Return");
            returnBtn.setOnAction(e -> window.setScene(homeScene));
            Button runBtn = new Button("Run");
                       
            
            Text text = new Text(b.getText());
            box.getChildren().add(text);
            
            // Get options from folder
            File mediaDir = new File(feature.getParentFile().getAbsolutePath()+"/media");
            
            if(mediaDir.exists())
            {
                for(File img : mediaDir.listFiles())
                {
                    if(getExtension(img).equals(new String("png")) || getExtension(img).equals(new String("jpg"))){
                        optionList.add(shortName(img));
                        featOptions.get(i).add(img);
                    }
                    
                }
            }
            else
                throw new Exception("No media folder found");
            
            final ChoiceBox choiceBox = new ChoiceBox(optionList);
            
            Label label = new Label();
            
            choiceBox.getSelectionModel().selectedItemProperty()
            .addListener((ObservableValue observable, 
                    Object oldValue, Object newValue) -> {
                label.setText((String)newValue);                
            });
            /*         WE HAVE TO CORRECT THIS LATER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DON'T ERASE
            choiceBox.getSelectionModel().selectedIndexProperty().addListener(new ChangeListener<Number>(){
                @Override
                public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue) {
                    index[0] = newValue.intValue();
                    choiceBox.getSelectionModel().select(index[0]);
                    
                }
            });            
            choiceBox.getSelectionModel().selectFirst();
            */
                  
                        
            
            runBtn.setOnAction(new EventHandler<ActionEvent>() {
                @Override public void handle(ActionEvent e) {
                    try
                    {
                        System.out.println(ENGINE_DIR+" "+shortName(feature)+" "+IMAGE_DIR+" "+mediaDir+"/"+label.getText()+".jpg");
                        final ProcessBuilder  pb = new ProcessBuilder(ENGINE_DIR, shortName(feature),IMAGE_DIR, mediaDir+"/"+label.getText()+".jpg");
                        //final ProcessBuilder  pb = new ProcessBuilder("bash touch OI.txt");
                        Process p = pb.start();     // Start the process.
                        System.out.println("Process started!");
                        p.waitFor();                // Wait for the process to finish.
                        System.out.println("Process finished!");
                    }
                    catch(IOException | InterruptedException exc)
                    {
                        exc.printStackTrace();
                    }
                    
                }
            });       
       
            box.getChildren().add(choiceBox);            
            box.getChildren().add(runBtn);
            box.getChildren().add(returnBtn);
            
            
            
            Scene s = new Scene(box, 300, 250);
            scenes[counter] = s;
            b.setOnAction(e -> window.setScene(s));
            counter++;
            
            
            
            
            
            
        }
        
        
        
        homeLayout.getChildren().addAll(features);
        homeLayout.setAlignment(Pos.CENTER);



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

    private String shortName(File f)
    {
        return f.getName().substring(0, f.getName().lastIndexOf('.'));
    }
    private String getExtension(File f)
    {
        return f.getName().substring(f.getName().lastIndexOf('.')+1, f.getName().length());
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }

}
