/* 
 * File:   GuiWindow.cpp
 * Author: keoma
 * 
 * Created on March 5, 2014, 11:53 AM
 */

#include "GuiWindow.h"
#include "GuiPresenter.h"
#include "Parameters.h"
#include <GL/gl.h>

GuiWindow::GuiWindow()
{
    _gtkWindow = build();
    
    g_signal_connect( _dstCanvas    , "configure-event", G_CALLBACK( cb_configGLCanvas )    , this );
    g_signal_connect( _dstCanvas    , "expose-event"   , G_CALLBACK( cb_exposeGLCanvas )    , this );
    g_signal_connect( _srcCanvas    , "configure-event", G_CALLBACK( cb_configGLCanvas )    , this );
    g_signal_connect( _srcCanvas    , "expose-event"   , G_CALLBACK( cb_exposeGLCanvas )    , this );
    
    _presenter = 0;
}



GuiWindow::~GuiWindow()
{
}

void GuiWindow::setPresenter( GuiPresenter* presenter )
{
    _presenter = presenter;
}

void GuiWindow::show()
{
    gtk_widget_show_all( _gtkWindow );
}



void GuiWindow::redraw()
{
    cb_exposeGLCanvas( _srcCanvas, NULL, this );
    cb_exposeGLCanvas( _dstCanvas, NULL, this );
}



GtkWidget* GuiWindow::build()
{
    //create window
    GtkWidget* window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    
    //create mainBox
    GtkWidget* mainBox = buildMainBox();
    
    //insert mainBox as outer most layout
    gtk_container_add( GTK_CONTAINER( window ), mainBox );
    
    return window;
}


GtkWidget* GuiWindow::buildMainBox()
{
    GtkWidget* mainBox = gtk_hbox_new( FALSE, 5 );
    
    GtkWidget* canvasBox = buildCanvasBox();
    gtk_box_pack_start( GTK_BOX( mainBox ), canvasBox, TRUE, TRUE, 5 );
    
    GtkWidget* controlsBox = buildControlsBox();
    gtk_box_pack_start( GTK_BOX( mainBox ), controlsBox, FALSE, FALSE, 5 );
    
    return mainBox;
}


GtkWidget* GuiWindow::buildCanvasBox()
{
    GtkWidget* canvasBox = gtk_hbox_new( TRUE, 1 );
    
    // create the drawing areas
    _dstCanvas = gtk_drawing_area_new();
    gtk_drawing_area_size( GTK_DRAWING_AREA( _dstCanvas ), 400, 100 );
    _srcCanvas = gtk_drawing_area_new();
    gtk_drawing_area_size( GTK_DRAWING_AREA( _srcCanvas ), 400, 100 );
    
    // OpenGL configuration for the canvas
    GdkGLConfig* glconfig = gdk_gl_config_new_by_mode (static_cast<GdkGLConfigMode>
                                                       (GDK_GL_MODE_RGB  |
                                                        GDK_GL_MODE_DEPTH|
                                                        GDK_GL_MODE_DOUBLE) );
    
    // Define the canvas as capable of rendering OpenGL graphics
    gtk_widget_set_gl_capability( _dstCanvas, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE );
    gtk_widget_set_gl_capability( _srcCanvas, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE );
    
    // Add Drawing Areas to the box
    gtk_box_pack_start_defaults( GTK_BOX( canvasBox ), _srcCanvas );
    gtk_box_pack_start_defaults( GTK_BOX( canvasBox ), _dstCanvas );
    
    return canvasBox;
}

GtkWidget* GuiWindow::buildNotebook()
{
    GtkWidget* notebook = gtk_notebook_new();
    
    //Pre Processing Page
    GtkWidget* preProcPage = buildPreProcPage();
    GtkWidget* preProcLabel = gtk_label_new( "Pré Proc." );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), preProcPage, preProcLabel );
    
    // Ants Page
    GtkWidget* paramPage = buildParametersPage();
    GtkWidget* paramLabel = gtk_label_new( "Params." );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), paramPage, paramLabel );
    
    // Post Processing page
    GtkWidget* visPage = buildPostProcPage();
    GtkWidget* visLabel = gtk_label_new( "Pós Proc." );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), visPage, visLabel );
    
    return notebook;
}

GtkWidget* GuiWindow::buildParametersPage()
{
    GtkWidget* notebook = gtk_notebook_new();
    
    // Ants Page
    GtkWidget* antsPage = buildAntsPage();
    GtkWidget* antsLabel = gtk_label_new( "Formiga" );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), antsPage, antsLabel );
    
    
    // Pheromone Page
    GtkWidget* pheromonePage = buildPheromonePage();
    GtkWidget* pheromoneLabel = gtk_label_new( "Feromônio" );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), pheromonePage, pheromoneLabel );
    
    
    // Direction Page
    GtkWidget* directionPage = buildDirectionPage();
    GtkWidget* directionLabel = gtk_label_new( "Direção" );
    gtk_notebook_append_page( GTK_NOTEBOOK(notebook), directionPage, directionLabel );
    
    return notebook;
}


GtkWidget* GuiWindow::buildAntsPage()
{
    GtkWidget* antsPage = gtk_vbox_new(FALSE, 3);
    
    GtkWidget* antsBox = buildParameterBox("Quantidade:", 1, 10000, 5, &Parameters::numberOfAnts, true );
    gtk_box_pack_start( GTK_BOX( antsPage ), antsBox, FALSE, TRUE, 2 );
    
    GtkWidget* freedomBox = buildParameterBox("Ângulo Visão:", 1, 180, 1, &Parameters::fov, false );
    gtk_box_pack_start( GTK_BOX( antsPage ), freedomBox, FALSE, TRUE, 2 );
    
    GtkWidget* alphaBox = buildParameterBox("Peso Feromônio:", 1, 10, 1, &Parameters::pheromoneWeight, false );
    gtk_box_pack_start( GTK_BOX( antsPage ), alphaBox, FALSE, TRUE, 2 );
    
    GtkWidget* etaBox = buildParameterBox("Peso Atributo:", 1, 10, 1, &Parameters::attributeWeight, false );
    gtk_box_pack_start( GTK_BOX( antsPage ), etaBox, FALSE, TRUE, 2 );
    
    GtkWidget* constantBox = buildParameterBox("Constante Feromônio:", 0.1, 10, 0.5, &Parameters::pheromoneConstant, false );
    gtk_box_pack_start( GTK_BOX( antsPage ), constantBox, FALSE, TRUE, 2 );
    
    GtkWidget* stepLengthBox = buildParameterBox("Tamanho Passo:", 1.0, 100, 1.0, &Parameters::stepLength, true );
    gtk_box_pack_start( GTK_BOX( antsPage ), stepLengthBox, FALSE, TRUE, 2 );
    
    GtkWidget* maxStepsBox = buildParameterBox("Número de Passos:", 1, 10000, 1.0, &Parameters::maxSteps, true );
    gtk_box_pack_start( GTK_BOX( antsPage ), maxStepsBox, FALSE, TRUE, 2 );
    
    GtkWidget* maxWrongStepsBox = buildParameterBox("Número de Passos Errôneos:", 1, 10000, 1, &Parameters::maxAbnormalSteps, true );
    gtk_box_pack_start( GTK_BOX( antsPage ), maxWrongStepsBox, FALSE, TRUE, 2 );
    
    return antsPage;
}

GtkWidget* GuiWindow::buildPheromonePage()
{
    GtkWidget* pheromonePage = gtk_vbox_new(FALSE, 3);
    
    GtkWidget* turnsBox = buildParameterBox("Turnos:", 1, 10000, 1, &Parameters::turns, true );
    gtk_box_pack_start( GTK_BOX( pheromonePage ), turnsBox, FALSE, TRUE, 2 );
    
    GtkWidget* evaporationBox = buildParameterBox("Taxa Evaporação:", 0.01, 1.0, 0.01, &Parameters::evaporationRate, false );
    gtk_box_pack_start( GTK_BOX( pheromonePage ), evaporationBox, FALSE, TRUE, 2 );
    
    GtkWidget* minPheromoneBox = buildParameterBox("Feromônio Mínimo:", 0.01, 1.0, 0.01, &Parameters::minPheromone, false );
    gtk_box_pack_start( GTK_BOX( pheromonePage ), minPheromoneBox, FALSE, TRUE, 2 );
    
    GtkWidget* maxPheromoneBox = buildParameterBox("Feromônio Máximo:", 0.1, 10000, 0.1, &Parameters::maxPheromone, false );
    gtk_box_pack_start( GTK_BOX( pheromonePage ), maxPheromoneBox, FALSE, TRUE, 2 );
    
    GtkWidget* startPheromoneBox = buildParameterBox("Feromônio Inicial:", 0.01, 10, 0.01, &Parameters::initPheromone, false );
    gtk_box_pack_start( GTK_BOX( pheromonePage ), startPheromoneBox, FALSE, TRUE, 2 );
   
    return pheromonePage;
}

GtkWidget* GuiWindow::buildDirectionPage()
{
    GtkWidget* directionPage = gtk_vbox_new(FALSE, 3);
    
    GtkWidget* wGaussBox = buildParameterBox("Largura Gauss:", 0, 10, 1, &Parameters::widthGauss, true );
    gtk_box_pack_start( GTK_BOX( directionPage ), wGaussBox, FALSE, TRUE, 2 );
    
    GtkWidget* hGaussBox = buildParameterBox("Altura Gauss:", 0, 10, 1, &Parameters::heightGauss, true );
    gtk_box_pack_start( GTK_BOX( directionPage ), hGaussBox, FALSE, TRUE, 2 );
    
    GtkWidget* cohTresholdBox = buildParameterBox("Limiar Consistência:", 0.0, 1.0, 0.01, &Parameters::cohTreshold, false );
    gtk_box_pack_start( GTK_BOX( directionPage ), cohTresholdBox, FALSE, TRUE, 2 );
    
    GtkWidget* kernelBox = buildParameterBox("Kernel Radius:", 0, 10, 1, &Parameters::kernelRadius, true );
    gtk_box_pack_start( GTK_BOX( directionPage ), kernelBox, FALSE, TRUE, 2 );
    
    return directionPage;
}

GtkWidget* GuiWindow::buildPostProcPage()
{
    GtkWidget* postProcPage = gtk_vbox_new(FALSE, 3);
    
    GtkWidget* stdDevBox = buildParameterBox("Desvios Padrão:", 0.0, 10.0, 0.5, &Parameters::postStdDev, false );
    gtk_box_pack_start( GTK_BOX( postProcPage ), stdDevBox, FALSE, TRUE, 2 );
    
    GtkWidget* gammaBox = buildParameterBox("Correção Gamma:", 0.0, 10.0, 0.1, &Parameters::gammaFactor, false );
    gtk_box_pack_start( GTK_BOX( postProcPage ), gammaBox, FALSE, TRUE, 2 );
    
    GtkWidget* binBox = buildParameterBox("Limiar de Binarização:", 0.00, 1.00, 0.01, &Parameters::binThreshold, false );
    gtk_box_pack_start( GTK_BOX( postProcPage ), binBox, FALSE, TRUE, 2 );
    
    GtkWidget* openKernelBox = buildParameterBox("Raio do kernel de abertura:", 0, 10, 1, &Parameters::openKernelRad, true );
    gtk_box_pack_start( GTK_BOX( postProcPage ), openKernelBox, FALSE, TRUE, 2 );
    
    GtkWidget* closeKernelBox = buildParameterBox("Raio do kernel de fechamento:", 0, 10, 1, &Parameters::closeKernelRad, true );
    gtk_box_pack_start( GTK_BOX( postProcPage ), closeKernelBox, FALSE, TRUE, 2 );
    
    GtkWidget* useBinarization = buildParameterBox( "Binarizar", &Parameters::binarization );
    gtk_box_pack_start( GTK_BOX( postProcPage ), useBinarization, FALSE, TRUE, 2 );
    
    GtkWidget* applyButton = gtk_button_new_with_label( "Aplicar" );
    gtk_box_pack_start( GTK_BOX( postProcPage ), applyButton, FALSE, TRUE, 2 );
    g_signal_connect( applyButton, "clicked", G_CALLBACK(cb_apply), this );
    
    GtkWidget* exportButton = gtk_button_new_with_label( "Exportar BMP" );
    gtk_box_pack_start( GTK_BOX( postProcPage ), exportButton, FALSE, TRUE, 2 );
    
    return postProcPage;
}

GtkWidget* GuiWindow::buildPreProcPage()
{
    GtkWidget* preProcPage = gtk_vbox_new(FALSE, 3);
    
    GtkWidget* stdDevBox = buildParameterBox("Desvios Padrão:", 0.0, 10.0, 0.5, &Parameters::preStdDev, false );
    gtk_box_pack_start( GTK_BOX( preProcPage ), stdDevBox, FALSE, TRUE, 2 );
    
    GtkWidget* useBinarization = buildParameterBox( "Manual", &Parameters::manualNormalization );
    gtk_box_pack_start( GTK_BOX( preProcPage ), useBinarization, FALSE, TRUE, 2 );
    
    GtkWidget* minBox = buildParameterBox("Mínimo:", 0.0, 10.0, 0.5, &Parameters::manualMin, false );
    gtk_box_pack_start( GTK_BOX( preProcPage ), minBox, FALSE, TRUE, 2 );
    
    GtkWidget* maxBox = buildParameterBox("Máximo:", 0.0, 10.0, 0.5, &Parameters::manualMax, false );
    gtk_box_pack_start( GTK_BOX( preProcPage ), maxBox, FALSE, TRUE, 2 );
    
    GtkWidget* applyButton = gtk_button_new_with_label( "Aplicar" );
    gtk_box_pack_start( GTK_BOX( preProcPage ), applyButton, FALSE, TRUE, 2 );
    g_signal_connect( applyButton, "clicked", G_CALLBACK(cb_apply), this );
    
    GtkWidget* exportButton = gtk_button_new_with_label( "Exportar BMP" );
    gtk_box_pack_start( GTK_BOX( preProcPage ), exportButton, FALSE, TRUE, 2 );
    
    return preProcPage;
}


GtkWidget* GuiWindow::buildControlsBox()
{
    GtkWidget* controlsBox = gtk_vbox_new( FALSE, 3 );
    
    GtkWidget* controlsLabel = gtk_label_new( "Controles:" );
    gtk_box_pack_start( GTK_BOX( controlsBox ), controlsLabel, FALSE, TRUE, 5 );
    
    GtkWidget* notebook = buildNotebook();
    gtk_box_pack_start( GTK_BOX( controlsBox ), notebook, FALSE, TRUE, 5 );
    
    GtkWidget* separator02 = gtk_hseparator_new();
    gtk_box_pack_start( GTK_BOX( controlsBox ), separator02, FALSE, TRUE, 5 );
    
    GtkWidget* loadButton = gtk_file_chooser_button_new( "Selecione faria de atributo sísmico", GTK_FILE_CHOOSER_ACTION_OPEN );
    g_signal_connect( loadButton, "file-set", G_CALLBACK(cb_fileChosen), this );
    gtk_box_pack_start( GTK_BOX( controlsBox ), loadButton, FALSE, TRUE, 2 );
    
    GtkWidget* runButton = gtk_button_new_with_label( "Executar" );
    g_signal_connect( runButton, "clicked", G_CALLBACK(cb_run), this );
    gtk_box_pack_start( GTK_BOX( controlsBox ), runButton, FALSE, TRUE, 2 );
    
    return controlsBox;
}

GtkWidget* GuiWindow::buildParameterBox(const char* name, bool* param)
{
    GtkWidget* paramBox = gtk_hbox_new( FALSE, 1 );
    GtkWidget* checkButton = gtk_check_button_new_with_label( name );
    
    gtk_box_pack_start( GTK_BOX(paramBox), checkButton, FALSE, TRUE, 1 );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(checkButton), *param );
    g_signal_connect( checkButton, "toggled", G_CALLBACK(cb_checkToggled), param );
    
    return paramBox;
}


GtkWidget* GuiWindow::buildParameterBox( const char* name, double min, double max, double step, void* param, bool isInteger )
{
    GtkWidget* paramBox = gtk_vbox_new( FALSE, 1 );
    GtkWidget* label = gtk_label_new( name );
    GtkWidget* spin = gtk_spin_button_new_with_range( min, max, step );
    
    gtk_box_pack_start( GTK_BOX(paramBox), label, FALSE, TRUE, 1 );
    gtk_box_pack_start( GTK_BOX(paramBox), spin,  TRUE, TRUE, 1 );
    
    if (isInteger)
    {
        int* iParam = (int*) param;
        gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin), *iParam );
        g_signal_connect( spin, "value-changed", G_CALLBACK(cb_intParamChanged), param );
    }
    else
    {
        float* fParam = (float*) param;
        gtk_spin_button_set_value( GTK_SPIN_BUTTON(spin), *fParam );
        g_signal_connect( spin, "value-changed", G_CALLBACK(cb_floatParamChanged), param );
    }
    
    return paramBox;
}



gboolean GuiWindow::cb_configGLCanvas( GtkWidget* canvas, GdkEventConfigure* event, gpointer user_data )
{
    GdkGLContext *glContext = gtk_widget_get_gl_context( canvas );
    GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable( canvas );
    
    // start opengl commands
    if( !gdk_gl_drawable_gl_begin( glDrawable, glContext ) )
    {
        g_assert_not_reached();
    }
    
    glEnable(GL_TEXTURE_2D);
    
    // end opengl commands
    gdk_gl_drawable_gl_end( glDrawable );
    
    //everything fine
    return TRUE;
}



gboolean GuiWindow::cb_exposeGLCanvas( GtkWidget* canvas, GdkEventExpose* event, gpointer user_data )
{
    GdkGLContext *glContext = gtk_widget_get_gl_context( canvas );
    GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable( canvas );
 
    //tell gtk that we will start calling opengl functions
    if( !gdk_gl_drawable_gl_begin( glDrawable, glContext ) )
    {
        g_assert_not_reached();
    }
    
    //Clear the canvas with black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    //Recover access to the window
    GuiWindow* window = ( GuiWindow* )user_data;
    
    Image* image = NULL;
    
    if( canvas == window->_dstCanvas )
    {
        image = window->_presenter->getOutputImage();
    }
    else
    {
        image = window->_presenter->getInputImage();
    }
    
    //If Image loaded OK, start drawing
    if( image )
    {
        int width = imgGetWidth( image );
        int height = imgGetHeight( image );
        
        // prepare matrix
        glViewport( 0, 0, canvas->allocation.width, canvas->allocation.height );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        //glOrtho ( 0, canvas->allocation.width, 0, canvas->allocation.height, -1, 1 );
        window->fit( width, height, canvas->allocation.width, canvas->allocation.height );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        GLuint tID = window->buildTexture( image );
        
        glBegin( GL_QUADS );    
            glTexCoord2f(0,0);
            glVertex2f(0,0);
            
            glTexCoord2f(1.0, 0.0);
            glVertex2f(width, 0);
            
            glTexCoord2f(1.0, 1.0);
            glVertex2f(width, height);
            
            glTexCoord2f(0.0, 1.0);
            glVertex2f(0, height);
        glEnd();
        
        glDeleteTextures( 1, &tID );
    }
    
    //Update image
    gdk_gl_drawable_swap_buffers (glDrawable);
    
    //Tell GTK we stopped messing with OpenGL
    gdk_gl_drawable_gl_end( glDrawable );
    
    // release images
    imgDestroy( image );
    
    //everything went fine
    return TRUE;
}

void GuiWindow::fit(int w, int h, int canvasW, int canvasH)
{
    float aspectRatio = (float)canvasW / (float)canvasH;

    // Computa a altura e largura da caixa envolvente
    float width  = w;
    float height = h;

    // Computa a coordenada do centro da caixa envolvente
    float cx = 0.5f* width;
    float cy = 0.5f* height;


    if (height != 0)
    {
        float aabbAspectRatio = width/height;

        if ( aspectRatio/aabbAspectRatio < 1)
        {
            height = width / aspectRatio;
        }
        else
        {
            width = height * aspectRatio;
        }
    }
    else
    {
        height = width / aspectRatio;
    }

    float xMin = cx - 0.5f*width;
    float xMax = cx + 0.5f*width;
    float yMin = cy - 0.5f*height;
    float yMax = cy + 0.5f*height;
    
    glOrtho( xMin, xMax, yMin, yMax, -1, 1 );
}


unsigned int GuiWindow::buildTexture(Image* img)
{
    // create texture object for the image
    GLuint textureID;
    glGenTextures( 1, &textureID);
    glBindTexture( GL_TEXTURE_2D, textureID );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );        
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
       
    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, imgGetWidth( img ), imgGetHeight( img ), 0, GL_LUMINANCE, GL_FLOAT, imgGetData( img ) );
    
    return textureID;
}




void GuiWindow::cb_intParamChanged( GtkSpinButton* spinbutton, gpointer user_data )
{
    int* param = (int*) user_data;
    
    *param = gtk_spin_button_get_value_as_int( spinbutton );
}



void GuiWindow::cb_floatParamChanged( GtkSpinButton* spinbutton, gpointer user_data )
{
    float* param = (float*) user_data;
    
    *param = gtk_spin_button_get_value_as_float( spinbutton );
}



void GuiWindow::cb_fileChosen( GtkFileChooserButton* widget, gpointer user_data )
{
    gchar* fileName = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(widget) );
    GuiWindow* window = (GuiWindow*) user_data;
    window->_presenter->loadImage( (char*)fileName );
    g_free( fileName );
}



void GuiWindow::cb_run( GtkButton* widget, gpointer user_data )
{
    GuiWindow* window = (GuiWindow*) user_data;
    window->_presenter->run();
}

void GuiWindow::cb_checkToggled(GtkToggleButton* togglebutton, gpointer user_data)
{
    bool* param = (bool*) user_data;
    if (gtk_toggle_button_get_active( togglebutton ) == TRUE)
    {
        *param = true;
    }
    else
    {
        *param = false;
    }
}

void GuiWindow::cb_apply(GtkButton* button, gpointer user_data)
{
    GuiWindow* window = (GuiWindow*) user_data;
    window->redraw();
}
