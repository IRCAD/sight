/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

package ${ANDROID_APK_PACKAGE};

import android.util.Log;

import android.view.View;
import android.view.WindowManager.LayoutParams;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.SeekBar;
import android.widget.Button;

import android.view.ViewGroup.MarginLayoutParams;
import android.view.Window;

import android.app.NativeActivity;
import android.view.Gravity;

public class ViewManager
{

    private static final String TAG = "ViewManager";
    private PopupWindow m_popupWindow;
    private LinearLayout m_layout;
    private NativeActivity m_activity;
    
    public ViewManager(NativeActivity activity)
    {
        Log.i(TAG, "ViewManager ");
        m_activity = activity;

        // Setup relative layout
        m_popupWindow = new PopupWindow(
                        LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        
       m_layout = new LinearLayout(m_activity);
       LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                              LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT);
       layoutParams.setMargins(0, 0, 0, 0);
       m_layout.setLayoutParams(layoutParams);
       m_layout.setGravity(Gravity.CENTER);
       m_layout.setOrientation(LinearLayout.VERTICAL);
       
       int padding = converDPToPixels(50);
       m_layout.setPadding(padding,padding,padding,padding);
    }
    
    public void update()
    {
         Log.i(TAG, "update ");
         
         
        m_activity.runOnUiThread(new Runnable()  
        {
            @Override
            public void run()  
            {
                Window window = m_activity.getWindow();
                if( window != null )
                {
                    View popupView = window.getDecorView();
                    if( popupView == null )
                      {
                          // Put dummy layout to NativeActivity
                          LinearLayout mainLayout = new LinearLayout(m_activity);
                          MarginLayoutParams params = new MarginLayoutParams(
                                  LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                          params.setMargins(0, 0, 0, 0);
                          m_activity.setContentView(mainLayout, params);
                          popupView = mainLayout;
                      }

                     // Set view
                      m_popupWindow.setContentView(m_layout);
                      Log.i(TAG, "create windows");

                      // Show our UI over NativeActivity window
                      m_popupWindow.showAtLocation(popupView, Gravity.TOP| Gravity.LEFT, 0, 0);
                      m_popupWindow.update();
                 }

            }
        });
    }
    
    public void createSlider(int max)
    {
        Log.i(TAG, "createSlider ");
        VerticalSeekBar seekBar = new VerticalSeekBar(m_activity);
 
        LinearLayout.LayoutParams seekParam = new  LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
          seekBar.setLayoutParams(seekParam);
          
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser)
            {
                Log.i(TAG, "progress =" + progress);
                // sliderValueChanged(0,progress);
            }
        });
        m_layout.addView(seekBar);
        
    }
    
    public void createButton(String label)
    {
        Log.i(TAG, "createButton");
        Button button = new Button(m_activity);
        button.setText(label);
        button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Log.i(TAG, "clicked ");
                // changeColor(true);
            }
        });
        m_layout.addView(button);
        
    } 
    
    public int converDPToPixels(int dp)
    {
        final float scale = m_activity.getResources().getDisplayMetrics().density;
        int pixels = (int) (dp * scale + 0.5f);
        return pixels;
    }
    //
    // public static native void sliderValueChanged(int id,int value);
   
}

