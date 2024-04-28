// otrData.h
#ifndef OTRDATA_H
#define OTRDATA_H

#include "otrFilehandling.h"
#include "otrDisplay.h"
#include <CSV_Parser.h>

extern TFT_eSPI tft;

extern char RFID[17];
extern String NLISID;
extern String VisualID;
extern String currentStatus;
extern String NAME;
extern String GROUP;
extern String LOCATION;
extern String COLOUR;
extern char **tagRFID;
extern char **tempRFID;
extern char **tagVisual_ID;
extern char **tagNLISID;
extern char **tagColour;
extern char **tagAppliedDate;
extern int numStatuses;
extern int numGroups;
extern int numLocations;
extern String tagStatuses[10];
extern String groups[20];
extern String locations[10];
extern String gender[3];
extern char **tagName;
extern uint8_t *tagStatusVal;
extern uint8_t *tagGender;
extern uint8_t *tagGroup;
extern uint8_t *tagLocation;
extern uint16_t numTags;
extern uint16_t numActiveTags;

bool read_tags_file(void);
void read_tag_group_list();
void read_tag_location_list(); 
void read_active_tags_file(void);
void read_tag_status_list(void);
void read_bucket_file(int row, char** bucketRow, int& bucketTags);


#endif