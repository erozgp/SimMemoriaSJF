#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "sim.h"
 
#define INICIAR 1
#define DETENER 0

int edosim = DETENER;
GtkWidget * acercade  = NULL;
GdkDrawable *pixmap = NULL;
gboolean bCol = FALSE;
GdkGC *gc[3];


GdkGC *GetPen(GdkDrawable *pxm, int R, int G, int B){
	//valores de 0 a 65535
	GdkGC *gc;
	GdkColor *c;
	c = (GdkColor *) g_malloc(sizeof(GdkColor));
	c -> red = R;
	c -> green = G;
	c -> blue = B;
	
	gdk_color_alloc(gdk_colormap_get_system(), c);
	
	gc = gdk_gc_new(pxm);
	gdk_gc_set_foreground(gc, c);
	
	return gc;
	
}	

void draw_text(GdkDrawable *pixmap, GdkGC *gc,  gchar *s_utf8, int x, int y, GdkColor gcolor, gchar * font)
{
	PangoLayout *layout;
	PangoFontDescription *desc;
	PangoAttrList * attrs;
	PangoAttribute * color;

	color = pango_attr_foreground_new  (gcolor.red, gcolor.green, gcolor.blue);
	layout = pango_layout_new(gdk_pango_context_get());
	attrs = pango_attr_list_new ();
	pango_attr_list_insert (attrs, color);	
	pango_layout_set_attributes (layout, attrs);
                 	
	desc = pango_font_description_from_string (font);
	pango_layout_set_font_description (layout, desc);
	
	pango_layout_set_text(layout, s_utf8, -1);

	gdk_draw_layout(pixmap, gc, x, y, layout);


	pango_attribute_destroy(color);
	g_object_unref(layout);
}

gboolean trazar(GtkWidget * widget){
	GdkRectangle update_rect;
	GtkWidget *where_to_draw = lookup_widget(widget, "drawingarea1");
	GdkColor gcolor1 = {0, 0xffff, 0x0000, 0xffff};
	GdkColor gcolor2 = {0, 0x7777, 0x00ff, 0xff77};
	int i, j, px, py;
	gboolean bpintar;
	char szMensaje[64];
	
	gdk_draw_rectangle(pixmap, widget->style->white_gc,
										TRUE, 
										0, 0,
										where_to_draw->allocation.width,
										where_to_draw->allocation.height);
	if(edosim == INICIAR){
		pasarAsw();
		ordenar();
		
		edosim = ejecutar(ipej);
		ipej >= contpa? ipej = 0 : ipej++;
		
	}
	
	if(!bmem){
		inicializarmem(&MemRam);
		inicializarmem(&MemSwap);
		bmem = TRUE;
	}
	
	if(!bCol){
		gc[0] = GetPen(pixmap, 65535/2, 0, 0);
		gc[1] = GetPen(pixmap, 0, 65535/2, 0);
		gc[2] = GetPen(pixmap, 0, 0, 65535/2);
		bCol = TRUE;
	}
	
	sprintf(szMensaje,"Memoria Ram");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 400, 10, gcolor1, "Serif 15");
	for(i=0; i< NMMM; i++){
		sprintf(szMensaje, "%2d", i);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 380, 40+i*15, gcolor2, "Serif 10");
		if(MemRam.marcos[i].pid == -1){
			gdk_draw_rectangle(pixmap, widget->style->black_gc,
										FALSE, 
										400, 40+i*15,
										100, 15);
		}else{
			gdk_draw_rectangle(pixmap, gc[MemRam.marcos[i].tip],
										TRUE, 
										400, 40+i*15,
										101, 15);
			sprintf(szMensaje, "pid:%d", MemRam.marcos[i].pid);
			draw_text(pixmap, widget->style->black_gc, szMensaje, 380+128, 40+i*15, gcolor2, "Serif 10");
		}
	}
	sprintf(szMensaje, "No marcos libres: %d", marcoslibres(MemRam));
	draw_text(pixmap, widget->style->black_gc, szMensaje, 380, 50+i*15, gcolor1, "Serif 11");
	
	px = 20;
	py = 40;
	for(i=0; i<contpa; i++){
		sprintf(szMensaje, "PID: %d", ProcesosAc[i].idp);
		draw_text(pixmap, widget->style->black_gc, szMensaje, px, py, gcolor1, "Serif 10");
		for(j=0; j<ProcesosAc[i].maras; j++){
			gdk_draw_rectangle(pixmap, gc[ProcesosAc[i].mapam[j]],
										TRUE, 
										px, 15+py+j*15,
										101, 15);
			sprintf(szMensaje, "cp ->: %d", ProcesosAc[i].cp);
		    draw_text(pixmap, widget->style->black_gc, szMensaje, px+2, 17+py+ProcesosAc[i].cp*15, gcolor2, "Serif 10");
		}
		py+=30 + 15*ProcesosAc[i].maras;
		if(i+1 < contpa){
			if(py+30+15*ProcesosAc[i].maras > 40+32*15){
				py = 40;
				px = 131;
			}
		}
	}
	sprintf(szMensaje, "Memoria Swap usada: %d",NMMM - marcoslibres(MemSwap));
	draw_text(pixmap, widget->style->black_gc, szMensaje, 20, 50+32*15, gcolor1, "Serif 11");
	sprintf(szMensaje, "Lista de procesos en espera: ");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 20, 70+32*15, gcolor1, "Serif 11");
	for(i=0; i < contps; i++){
		sprintf(szMensaje, "%d ->",ProcesosSw[i].idp);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 20+i*45, 85+32*15, gcolor1, "Serif 11");
	}
	sprintf(szMensaje, "NULL");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 20+i*45, 85+32*15, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "TICKS de tiempo = %d ", ticks);
	draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 110, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "TABLA DE LLEGADA/ESPERA");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 210, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "| PID |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 237, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "| Tiempo |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 727, 237, gcolor1, "Serif 11");
						
	sprintf(szMensaje, "| Tamaño |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 807, 237, gcolor1, "Serif 11");

	
	for(i = 0; i < contesp; i++){
		
		sprintf(szMensaje, "%d", Espera[i].idp);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 257+i*20, gcolor1, "Serif 10");
	
		sprintf(szMensaje, "%d", Espera[i].tiempo);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 727, 257+i*20, gcolor1, "Serif 10");
	
		sprintf(szMensaje, "%d", Espera[i].npc);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 807, 257+i*20, gcolor1, "Serif 10");
		
	}
	sprintf(szMensaje, "	TABLA SWAP");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 410, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "| PID |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 437, gcolor1, "Serif 11");
	
	sprintf(szMensaje, "| Tiempo |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 727, 437, gcolor1, "Serif 11");
						
	sprintf(szMensaje, "| Tamaño |");
	draw_text(pixmap, widget->style->black_gc, szMensaje, 807, 437, gcolor1, "Serif 11");
	for(i = 0; i < contps; i++){
		
		sprintf(szMensaje, "%d", ProcesosSw[i].idp);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 647, 457+i*20, gcolor1, "Serif 10");
	
		sprintf(szMensaje, "%d", ProcesosSw[i].tiempo);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 727, 457+i*20, gcolor1, "Serif 10");
	
		sprintf(szMensaje, "%d", ProcesosSw[i].npc);
		draw_text(pixmap, widget->style->black_gc, szMensaje, 807, 457+i*20, gcolor1, "Serif 10");
		
	}
	
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = where_to_draw->allocation.width;
	update_rect.height = where_to_draw->allocation.height;
	
	gtk_widget_draw(where_to_draw, &update_rect);
	
	return TRUE;
}

void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_toolbuttonEjecutar_clicked          (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	int i; 
	/*for(i = 0; i<contpa; i++){
		ProcesosAc[i].edo = EJEC;
	}*/
	/*
	ordenar();
	if(ProcesosSw[0].tiempo == 0){
		ordSJFconCero();
		printf("Ordena de acuerdo al tamaño\n, pero ignora el indice 0\n");
		
	}else if(ProcesosSw[0].tiempo != 0){
		ordSJFsinCero();
		printf("Ordena TODOS de acuerdo al tamaño\n");
	}
	*/
	
	//printf("ANTES DE SWAPIN ProcesosSw[0].tiempo = %d\n", ProcesosSw[0].tiempo);
	ProcesosAc[0].edo = EJEC;
	ticks = 0;
	printf("ejecutando...\n");
	fflush(stdout);
	edosim = INICIAR;
	//printf("DESPUES DE SWAPIN ProcesosSw[0].tiempo = %d\n", ProcesosSw[0].tiempo);
}


void
on_toolbuttonAdicionarProceso_clicked  (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget * dlgAdicionar = NULL;
	GtkWidget * entryPID;
	GtkWidget * entryPagCod;
	GtkWidget * entryPagDat;
	GtkWidget * entryPagPil;
	GtkWidget * entryTiempo;
	char szPID[32];
	char szDato[32];
	int npc, npd, npp, t;
	int result;
	
	if(edosim == INICIAR){
		return;
	}
	
	dlgAdicionar = create_dialogAdicionar();
	entryPID = lookup_widget((GtkWidget *)dlgAdicionar, "entryPID");
	entryPagCod = lookup_widget((GtkWidget *)dlgAdicionar, "entryPagCod");
	entryPagDat = lookup_widget((GtkWidget *)dlgAdicionar, "entryPagDat");
	entryPagPil = lookup_widget((GtkWidget *)dlgAdicionar, "entryPagPil");
	entryTiempo = lookup_widget((GtkWidget *)dlgAdicionar, "entryTiempo");
	sprintf(szPID, "%d", idps);
	gtk_entry_set_text((GtkEntry *)entryPID, szPID);
	
	result = gtk_dialog_run(GTK_DIALOG(dlgAdicionar));
	odenarTabla();
	switch(result){
		case GTK_RESPONSE_OK:
			npc = atoi(gtk_entry_get_text((GtkEntry *)entryPagCod));
			npd = atoi(gtk_entry_get_text((GtkEntry *)entryPagDat));
			npp = atoi(gtk_entry_get_text((GtkEntry *)entryPagPil));
			t = atoi(gtk_entry_get_text((GtkEntry *)entryTiempo));
			/*if(!adicionarSW(npc, npd, npp,t)){
				printf("No hay suficiente memoria para\nun nuevo proceso \nLa adición no pudo ser completada!!\n");
			}*/
			esperaDeSw(npc,npd,npp,t);
			odenarTabla();
			printf("Adicionar hecho\nuwu\n");
		break;
		default:
			printf("Operación cancelada\n");
			odenarTabla();
		break;
	}
	trazar((GtkWidget *) toolbutton);
	gtk_widget_destroy(dlgAdicionar);
}


void
on_toolbuttonAcercade_clicked          (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	int var;
	
	acercade = create_aboutdialog();
	gtk_about_dialog_set_version((GtkAboutDialog *)acercade,"1.0");
	var = gtk_dialog_run(GTK_DIALOG(acercade));
	
	//var = GTK_RESPONSE_CANCEL;

	//gtk_widget_show(acercade);
	gtk_widget_destroy(acercade);
}


void
on_toolbuttonSalir_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	on_window1_destroy((GtkObject *)toolbutton, user_data);
}


gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{

  if(pixmap != NULL)
		gdk_pixmap_unref(pixmap);
		
	pixmap = gdk_pixmap_new(widget->window,
							widget->allocation.width,
							widget->allocation.height,
							-1);
	trazar(widget);
	
	return TRUE;
}


gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
gdk_draw_pixmap(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
					pixmap,
					event->area.x, event->area.y,
					event->area.x, event->area.y,
					event->area.width, event->area.height);
					
	return FALSE;
}


void
on_aboutdialog_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
	acercade = NULL;
}

