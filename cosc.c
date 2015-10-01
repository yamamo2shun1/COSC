/*
 * Copylight (C) 2015, Shunichi Yamamoto, tkrworks.net
 *
 * This file is part of COSC.
 *
 * COSC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option ) any later version.
 *
 * COSC is distributed in the hope that it will be useful,
 * but WITHIOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with COSC. if not, see <http:/www.gnu.org/licenses/>.
 *
 * cosc.c,v.0.1.0 2015/10/01
 */

#include "cosc.h"

static int sd;
static struct sockaddr_in addr;
static int oscTotalSize;
static char sndOSCData[MAX_MESSAGE_LEN];

void setOSCAddress(const char* prefix, const char* command);
void setOSCTypeTag(const char* type);
void addOSCIntArgument(int value);
void addOSCFloatArgument(float value);
void addOSCStringArgument(char* str);
void clearOSCMessage(void);
int flushOSCMessage(void);

int main(int argc, char *args[])
{
    int i = 0;
    int len = 0;
    char* typetag = NULL;

    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket NG...");
        return -1;
    }

    if(argc < 4)
    {
        printf("Tarinai YO!");
        return -1;
    }
    
    addr.sin_family = AF_INET;

    for(i = 0; i < 5; i++)
    {
        switch(i)
        {
        case 1:
            addr.sin_addr.s_addr = inet_addr(args[i]);
            break;
        case 2:
            addr.sin_port = htons(atoi(args[i]));
            break;
        case 3:
            setOSCAddress("/cosc", args[i]);
            break;
        case 4:
            len = strlen(args[i]);
            typetag = args[i];
            setOSCTypeTag(args[i]);
            //printf("argument length = %d\n", len);
            break;
        }
        
        //printf("%d's argument = %s\n", i, args[i]);
    }

    for(i = 0; i < len; i++)
    {
        //printf("argument type = %c\n", args[4][i]);
        //printf("argument type = %c\n", typetag[i]);

        switch(typetag[i])
        {
        case 'i':
            addOSCIntArgument(atoi(args[5 + i]));
            break;
        case 'f':
            addOSCFloatArgument(atof(args[5 + i]));
            break;
        case 's':
            addOSCStringArgument(args[5 + i]);
            break;
        }
    }

    flushOSCMessage();
    
    return 0;
}

void setOSCAddress(const char* prefix, const char* command)
{
    int prefixSize = strchr(prefix, 0) - prefix;
    int commandSize = strchr(command, 0) - command;
    int addressSize = prefixSize + commandSize;
    int zeroSize = 0;

    memset(sndOSCData, 0, MAX_MESSAGE_LEN);
    oscTotalSize = 0;

    sprintf(sndOSCData, "%s%s", prefix, command);

    zeroSize = (addressSize ^ ((addressSize >> 3) << 3)) == 0 ? 0 : 8 - (addressSize ^ ((addressSize >> 3) << 3));
    if(zeroSize == 0)
        zeroSize = 4;
    else if(zeroSize > 4 && zeroSize < 8)
        zeroSize -= 4;

    oscTotalSize = (addressSize + zeroSize);
}

void setOSCTypeTag(const char* type)
{
    int typeSize = strchr(type, 0) - type;
    int zeroSize = 0;

    sprintf((sndOSCData + oscTotalSize), ",%s", type);

    typeSize++;
    zeroSize = (typeSize ^ ((typeSize >> 2) << 2)) == 0 ? 0 : 4 - (typeSize ^ ((typeSize >> 2) << 2));
    if(zeroSize == 0)
        zeroSize = 4;
        
    oscTotalSize += (typeSize + zeroSize);
}

void addOSCIntArgument(int value)
{
    sndOSCData[oscTotalSize++] = (value >> 24) & 0xFF;
    sndOSCData[oscTotalSize++] = (value >> 16) & 0xFF;
    sndOSCData[oscTotalSize++] = (value >> 8) & 0xFF;
    sndOSCData[oscTotalSize++] = (value >> 0) & 0xFF;
}

void addOSCFloatArgument(float value)
{
    char* fchar = NULL;
    fchar = (char *)&value;
    sndOSCData[oscTotalSize++] = fchar[3] & 0xFF;
    sndOSCData[oscTotalSize++] = fchar[2] & 0xFF;
    sndOSCData[oscTotalSize++] = fchar[1] & 0xFF;
    sndOSCData[oscTotalSize++] = fchar[0] & 0xFF;
}

void addOSCStringArgument(char* str)
{
    int cstr_len = 0;
    while(*str)
    {
        sndOSCData[oscTotalSize + cstr_len] = *(str++) & 0xFF;
        cstr_len++;
    }
    oscTotalSize += ((cstr_len / 4) + 1) * 4;
}

void clearOSCMessage(void)
{
    memset(sndOSCData, 0, MAX_MESSAGE_LEN);
    oscTotalSize = 0;
}

int flushOSCMessage(void)
{
    //printf("osc total size = %d\n", oscTotalSize);
    if(sendto(sd, sndOSCData, oscTotalSize, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    //if(sendto(sd, "Hello", 5, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("OSC sending failed...\n");
        return -1;
    }

    close(sd);
}
