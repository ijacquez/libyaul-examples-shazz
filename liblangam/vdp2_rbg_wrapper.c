
#include <yaul.h>
#include "vdp2_rbg_wrapper.h"

// Globals

uint16_t	gK_TableBuff[2][820];
uint32_t	gK_TableNum[2];
uint16_t	gK_TableFlag[2];
uint32_t	gRbgKtbAddr[2];
int32_t		gRbgKtbOffset[2];
uint32_t 	gRotateTableAddress;
fix32_t		gRotateXy[2];
fix32_t		gRotateZ[2];
fix32_t		gRotateDisp[2];
fix32_t		gRotateMoveZ[2];
fix32_t		gCsx[2],gCsy[2];

// toc checkl type and use
uint32_t    gRotXySw[2];

uint8_t		gRa,gRb;

uint16_t	gDisplayX = 320;
uint16_t	gDisplayY = 224;

// registers buffer
rot_t		g_r_reg;
rotreg_t	_gRotregBuff[2];
rotreg_t	* gRotregBuff = _gRotregBuff;

static	fix32_t	gCurrentMatrix[2][9];
uint16_t	gRotateTableMode=2;


/*------------------------------------------------------------------------
 *
 * NAME : initGlobals
 * 
 *------------------------------------------------------------------------
 */
void initGlobals(void)
{
    gRotateXy[0]    = FIXED(0);
    gRotateXy[1]    = FIXED(0);
    gRotateZ[0]     = FIXED(0);
    gRotateZ[1]     = FIXED(0);
    gRotateMoveZ[0] = FIXED(0);
    gRotateMoveZ[1] = FIXED(0);
    gK_TableFlag[0] = 0;
    gK_TableFlag[1] = 0;
    gRotXySw[0]     = 0;
    gRotXySw[1]     = 0;

    gRbgKtbAddr[0] = 0;
    gRbgKtbAddr[1] = 0;

    gRbgKtbOffset[0] = 0;
    gRbgKtbOffset[1] = 0;
	gRotateTableAddress = 0;

	/*
	 *	Rotate Scroll Extension Registers Area Initialization
	 */
	g_r_reg.paramode = 0;
	g_r_reg.paramcontrl = 0;
	g_r_reg.k_contrl = 0;
	g_r_reg.k_offset = 0;
	g_r_reg.mapover[0] = 0;
	g_r_reg.mapover[1] = 0;
	g_r_reg.paramaddr = 0;
}

/*------------------------------------------------------------------------
 *
 * NAME : initRotateTable
 *
 * PARAMETERS
 *  param1 - address
 *  param2 - mode
 *  param3 - rA
 *  param4 - rB
 * 
 *------------------------------------------------------------------------
 */
uint32_t vdp2_rbg_initRotateTable(uint32_t address, uint16_t mode, uint32_t rA, uint32_t rB)
{
	initGlobals();

    uint32_t  	addressW;
    fix32_t  	r;
    uint16_t	rpmd;

    if( (rA != RBG0) && (rA != NON) && !((rA == SPR) && (rB == RBG0)) )
		return(2);
    if( (rB != RBG0) && (rB != RBG1) && (rB != NON) )
		return(2);

    gRa = rA;
    gRb = rB;
    
    // set RPMD mode
    if( (rA == RBG0) && (rB == RBG0) )		rpmd = 2;
    else if( (rA == NON) && (rB == RBG0) )	rpmd = 1;
	//else if( (rA == SPR) && (rB == RBG0) )	rpmd = 3;*/
    else									rpmd = 0;

    g_r_reg.paramode = rpmd;    /* Rotation parameters mode bit */

    r=FIXED(0);

    gRotateDisp[0]=FIXED(0);
    gRotateDisp[1]=FIXED(0);

    gRotateXy[0] = FIXED(0);
    gRotateZ[0]  = FIXED(0);
    gRotateXy[1] = FIXED(0);
    gRotateZ[1]  = FIXED(0);

    gCsx[0] = FIXED( gDisplayX/2 );
    gCsy[0] = FIXED( gDisplayY/2 );

    gCsx[1] = gCsx[0];
    gCsy[1] = gCsy[0];

    gRotateTableAddress = address;
    
    // looks totally useless
    gRotateTableMode = mode;

    gRotregBuff[0].screenst.x = gCsx[0] - MUL_FIXED(Fcos(r),gCsx[0])-MUL_FIXED((-Fsin(r)),gCsy[0]);
    gRotregBuff[0].screenst.y = gCsy[0] - MUL_FIXED(Fsin(r),gCsx[0])-MUL_FIXED((Fcos(r)),gCsy[0]);
    gRotregBuff[0].screenst.z  =  FIXED(0);
    gRotregBuff[0].screendlt.x = -Fsin(r);
    gRotregBuff[0].screendlt.y =  Fcos(r);
    gRotregBuff[0].delta.x     =  Fcos(r);
    gRotregBuff[0].delta.y     =  Fsin(r);

    gRotregBuff[0].matrix_a    = FIXED(1);
    gRotregBuff[0].matrix_b    = FIXED(0);
    gRotregBuff[0].matrix_c    = FIXED(0);
    gRotregBuff[0].matrix_d    = FIXED(0);
    gRotregBuff[0].matrix_e    = FIXED(1);
    gRotregBuff[0].matrix_f    = FIXED(0);

    gRotregBuff[0].viewp.x     = gDisplayX/2;
    gRotregBuff[0].viewp.y     = gDisplayY/2;
    gRotregBuff[0].viewp.z     = 400;

    gRotregBuff[0].rotatecenter.x = gDisplayX/2;
    gRotregBuff[0].rotatecenter.y = gDisplayY/2;
    gRotregBuff[0].rotatecenter.z = 0;

    gRotregBuff[0].move.x      = FIXED(0);
    gRotregBuff[0].move.y      = FIXED(0);
    gRotregBuff[0].zoom.x      = FIXED(1);
    gRotregBuff[0].zoom.y      = FIXED(1);

    if(mode == 2)
    {
		gRotregBuff[1].screenst.x  = gRotregBuff[0].screenst.x;
		gRotregBuff[1].screenst.y  = gRotregBuff[0].screenst.y;
		gRotregBuff[1].screenst.z  = gRotregBuff[0].screenst.z;
		gRotregBuff[1].screendlt.x = gRotregBuff[0].screendlt.x;
		gRotregBuff[1].screendlt.y = gRotregBuff[0].screendlt.y;
		gRotregBuff[1].delta.x     = gRotregBuff[0].delta.x;
		gRotregBuff[1].delta.y     = gRotregBuff[0].delta.y;

		gRotregBuff[1].matrix_a    = gRotregBuff[0].matrix_a;
		gRotregBuff[1].matrix_b    = gRotregBuff[0].matrix_b;
		gRotregBuff[1].matrix_c    = gRotregBuff[0].matrix_c;
		gRotregBuff[1].matrix_d    = gRotregBuff[0].matrix_d;
		gRotregBuff[1].matrix_e    = gRotregBuff[0].matrix_e;
		gRotregBuff[1].matrix_f    = gRotregBuff[0].matrix_f;

		gRotregBuff[1].viewp.x     = gRotregBuff[0].viewp.x;
		gRotregBuff[1].viewp.y     = gRotregBuff[0].viewp.y;
		gRotregBuff[1].viewp.z     = gRotregBuff[0].viewp.z;

		gRotregBuff[1].rotatecenter.x = gRotregBuff[0].rotatecenter.x;
		gRotregBuff[1].rotatecenter.y = gRotregBuff[0].rotatecenter.y;
		gRotregBuff[1].rotatecenter.z = gRotregBuff[0].rotatecenter.z;

		gRotregBuff[1].move.x      = gRotregBuff[0].move.x;
		gRotregBuff[1].move.y      = gRotregBuff[0].move.y;
		gRotregBuff[1].zoom.x      = gRotregBuff[0].zoom.x;
		gRotregBuff[1].zoom.y      = gRotregBuff[0].zoom.y;
    }

    addressW = ((address & 0x0007ff80)>>1) + (address & 0x0000003e)/4;
    g_r_reg.paramaddr = addressW;

    return(0);
}


/*------------------------------------------------------------------------
 *
 * NAME : Fsin
 *
 * PARAMETERS
 *  param1 - angle
 *
 *------------------------------------------------------------------------
 */
fix32_t	Fsin(fix32_t a)
{
	uint16_t	sign;
	fix32_t	val;

	sign = 0;
	if(a & 0x80000000){
		sign = 1;
		a = - a;
	}
	a = a % FIXED(360); 	/* 0 <= a < 360  */

	if(FIXED(270) <= a){	/*  270 <= a < 360  */
		val = -fsin(FIXED(360) - a);
	}
	else if(FIXED(180) <= a){	/*  180 <= a < 270  */
		val = -fsin(a - FIXED(180));
	}
	else if(FIXED(90) <= a){	/*  90 <= a < 180  */
		val = fsin(FIXED(180) - a);
	}
	else{			/* 0 <= a < 90 */
		val = fsin(a);
	}

	if(val==0x0000ffff)	val = FIXED(1);

	if(sign)	val = -val;

	return(val);
}

/*------------------------------------------------------------------------
 *
 * NAME : Fsin
 *
 * PARAMETERS
 *  param1 - angle
 *
 *------------------------------------------------------------------------
 */
fix32_t	Fcos(fix32_t a)
{
	fix32_t	val;

	if(a & 0x80000000)
    {
		a = - a;
	}
	a = a % FIXED(360);		/* 0 <= a < 360  */

	if(a == 0)	return( FIXED(1) );

	if(FIXED(270) <= a){		/*  270 <= a < 360  */
		val = fcos(FIXED(360) - a);
	}else if(FIXED(180) < a){	/*  180 <  a < 270  */
		val = -fcos(a - FIXED(180));
	}else if(FIXED(180) == a){	/*  a == 180  */
		val = - FIXED(1);
	}else if(FIXED(90) <= a){	/*  90 <= a < 180  */
		val = -fcos(FIXED(180) - a);
	}else{				/* 0 <= a < 90 */
		val = fcos(a);
	}
	return(val);
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_moveTo
 *
 * PARAMETERS
 *  param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - pos X 
 *	param3 - pos Y
 *	param4 - pos Z
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_moveTo(uint32_t screen, fix32_t x, fix32_t y, fix32_t z)
{
	switch(screen) {
		case RBG_TB_A:
			gRotregBuff[0].move.x = x;
			gRotregBuff[0].move.y = y;
			if(gRbgKtbAddr[0] && !(gRbgKtbOffset[0] && gRotateDisp[0]) )
				gRotateMoveZ[0]   = z;
			else
				gRotateMoveZ[0]   = 0;
			if( /*(g_r_reg.k_contrl & 0x00ff) &&*/ gRotateMoveZ[0])
			    vdp2_rbg_rotate(screen, 0,0,0);
			break;
		case RBG_TB_B:
			gRotregBuff[1].move.x = x;
			gRotregBuff[1].move.y = y;
			if(gRbgKtbAddr[1] && !(gRbgKtbOffset[1] && gRotateDisp[1]) )
				gRotateMoveZ[1]   = z;
			else
				gRotateMoveZ[1]   = 0;
			if( /*(g_r_reg.k_contrl & 0xff00) &&*/ gRotateMoveZ[1] )
			    vdp2_rbg_rotate(screen, 0,0,0);
			break;
	}
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_move
 *
 * PARAMETERS
 *  param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - delta X  
 *	param3 - delta Y
 *	param4 - delta Z
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_move(uint32_t screen, fix32_t x, fix32_t y, fix32_t z)
{
	switch(screen) {
		case RBG_TB_A:
			gRotregBuff[0].move.x += x;
			gRotregBuff[0].move.y += y;
			if(gRbgKtbAddr[0] && !(gRbgKtbOffset[0] && gRotateDisp[0]) )
				gRotateMoveZ[0]   += z;
			else
				gRotateMoveZ[0]   = 0;
			if( /*(g_r_reg.k_contrl & 0x00ff) &&*/ gRotateMoveZ[0] )
			    vdp2_rbg_rotate(screen, 0,0,0);
			break;
		case RBG_TB_B:
			gRotregBuff[1].move.x += x;
			gRotregBuff[1].move.y += y;
			if(gRbgKtbAddr[1] && !(gRbgKtbOffset[1] && gRotateDisp[1]) )
				gRotateMoveZ[1]   += z;
			else
				gRotateMoveZ[1]   = 0;
			if( /*(g_r_reg.k_contrl & 0xff00) &&*/ gRotateMoveZ[1] )
			    vdp2_rbg_rotate(screen, 0,0,0);
			break;
	}
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_rotate
 *
 * PARAMETERS
 *  param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - XorY Angle
 *	param3 - Z Angle
 *	param4 - Display(Z) Angle
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_rotate(uint32_t screen, fix32_t xy, fix32_t z, fix32_t disp)
{
	uint16_t	tbNum;

	switch(screen)
	{
		case RBG_TB_A:
			tbNum = 0;
			break;
		case RBG_TB_B:
			tbNum = 1;
			break;
		default:
			return;
			break;
	}

	gCurrentMatrix[tbNum][0] = FIXED(1);
	gCurrentMatrix[tbNum][1] = FIXED(0);
	gCurrentMatrix[tbNum][2] = FIXED(0);
	gCurrentMatrix[tbNum][3] = FIXED(0);
	gCurrentMatrix[tbNum][4] = FIXED(1);
	gCurrentMatrix[tbNum][5] = FIXED(0);
	gCurrentMatrix[tbNum][6] = FIXED(0);
	gCurrentMatrix[tbNum][7] = FIXED(0);
	gCurrentMatrix[tbNum][8] = FIXED(1);

	if(gRbgKtbAddr[tbNum])
	{
		gRotateXy[tbNum] += xy;
	}
	else
	{
		gRotateXy[tbNum] = 0;
	}

	gRotateZ[tbNum]  += z;

	if(!gRotateXy[tbNum] && xy) 
    {
		switch(screen)
		{
			case RBG_TB_A:
				g_r_reg.k_contrl &= 0xff00;
				break;
			case RBG_TB_B:
				g_r_reg.k_contrl &= 0x00ff;
				break;
		}
	}

	if(disp) vdp2_rbg_rotateZD(screen, disp);

	if(gRbgKtbOffset[tbNum] && gRotateDisp[tbNum]) {
		gRotateXy[tbNum] = 0;
	}

	if(gRotateXy[tbNum] || gRotateMoveZ[tbNum])
	{
		gCurrentMatrix[tbNum][0] = gRotregBuff[tbNum].zoom.x;
		gCurrentMatrix[tbNum][4] = gRotregBuff[tbNum].zoom.y;
	}
	else if( (tbNum==0 && (g_r_reg.k_contrl & 0x00ff) )	|| (tbNum==1 && (g_r_reg.k_contrl & 0xff00)) )
	{
		gCurrentMatrix[tbNum][0] = gRotregBuff[tbNum].zoom.x;
		gCurrentMatrix[tbNum][4] = gRotregBuff[tbNum].zoom.y;
		gRotregBuff[tbNum].matrix_a  = gRotregBuff[tbNum].zoom.x;
		gRotregBuff[tbNum].matrix_e  = gRotregBuff[tbNum].zoom.y;
	}

	if(gRotateZ[tbNum])	vdp2_rbg_rotateZ(screen, gRotateZ[tbNum]);

	if( gRotateXy[tbNum] || gRotateMoveZ[tbNum] )
	{
		if(gRotXySw[tbNum]==0 || gRbgKtbOffset[tbNum])
			vdp2_rbg_rotateX(screen, gRotateXy[tbNum]);
		else
			vdp2_rbg_rotateY(screen, gRotateXy[tbNum]);
	}
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_rotateZD
 *
 * PARAMETERS
 *  param1 - Rotation Scroll Screen (0 or 1)
 *  param2 - r ?
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_rotateZD(uint32_t screen, fix32_t r)
{
	uint16_t tbNum;

	switch(screen)
	{
		case RBG_TB_A:
			tbNum = 0;
			break;
		case RBG_TB_B:
			tbNum = 1;
			break;
		default:
			return;
			break;
	}

	if(gRbgKtbOffset[tbNum] && (gRotateXy[tbNum] || gRotateMoveZ[tbNum]) )
	{
		gRotateDisp[tbNum] = 0;
		return;
	}

	gRotateDisp[tbNum] += r;

	if(gRotateDisp[tbNum] >= FIXED(360)) gRotateDisp[tbNum]-=FIXED(360);
	if(gRotateDisp[tbNum] < FIXED(0))    gRotateDisp[tbNum]+=FIXED(360);

	gRotregBuff[tbNum].screenst.x 	= gCsx[tbNum] - MUL_FIXED(Fcos(gRotateDisp[tbNum]),gCsx[tbNum])	- MUL_FIXED((-Fsin(gRotateDisp[tbNum])),gCsy[tbNum]);
	gRotregBuff[tbNum].screenst.y 	= gCsy[tbNum] - MUL_FIXED(Fsin(gRotateDisp[tbNum]),gCsx[tbNum]) - MUL_FIXED((Fcos(gRotateDisp[tbNum])),gCsy[tbNum]);
	gRotregBuff[tbNum].screenst.z  	= FIXED(0);
	gRotregBuff[tbNum].screendlt.x 	= -Fsin(gRotateDisp[tbNum]);
	gRotregBuff[tbNum].screendlt.y 	=  Fcos(gRotateDisp[tbNum]);
	gRotregBuff[tbNum].delta.x     	=  Fcos(gRotateDisp[tbNum]);
	gRotregBuff[tbNum].delta.y     	=  Fsin(gRotateDisp[tbNum]);
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_rotateX
 *
 * PARAMETERS
 *	param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - xAngle
 *
 * DESCRIPTION
 *
 * | M00 M01 M02 | | 1    0    0 |
 * | M10 M11 M12 |.| 0  cos -sin |
 * | M20 M21 M22 | | 0  sin  cos |
 *
 *		         | M00 M01*cos+M02*sin -M01*sin+M02*cos |
 *		       = | M10 M11*cos+M12*sin -M11*sin+M12*cos |
 *		         | M20 M21*cos+M22*sin -M21*sin+M22*cos |
 *
 *------------------------------------------------------------------------
 */
void  vdp2_rbg_rotateX(uint32_t screen, fix32_t xAngle)
{
    fix32_t		sinNum, cosNum;
    fix32_t		M01, M02, M11, M12, M21, M22;
    fix32_t		Zp,Zs_Zp,Zs_ZpW;
    uint32_t		i;
    uint16_t	tbNum;

    switch(screen)
    {
		case RBG_TB_A:
			tbNum = 0;
			g_r_reg.k_contrl &= 0xff00;
			g_r_reg.k_contrl |= 0x0003;
			break;
		case RBG_TB_B:
			tbNum = 1;
			g_r_reg.k_contrl &= 0x00ff;
			g_r_reg.k_contrl |= 0x0300;
			break;
		default:
			return;
			break;
    }

    sinNum = Fsin(xAngle);
    cosNum = Fcos(xAngle);
    M01 = gCurrentMatrix[tbNum][1];
    M02 = gCurrentMatrix[tbNum][2];
    M11 = gCurrentMatrix[tbNum][3+1];
    M12 = gCurrentMatrix[tbNum][3+2];
    M21 = gCurrentMatrix[tbNum][6+1];
    M22 = gCurrentMatrix[tbNum][6+2];
    gCurrentMatrix[tbNum][1]   =  MUL_FIXED(M01, cosNum) + MUL_FIXED(M02, sinNum);
    gCurrentMatrix[tbNum][2]   = -MUL_FIXED(M01, sinNum) + MUL_FIXED(M02, cosNum);
    gCurrentMatrix[tbNum][3+1] =  MUL_FIXED(M11, cosNum) + MUL_FIXED(M12, sinNum);
    gCurrentMatrix[tbNum][3+2] = -MUL_FIXED(M11, sinNum) + MUL_FIXED(M12, cosNum);
    gCurrentMatrix[tbNum][6+1] =  MUL_FIXED(M21, cosNum) + MUL_FIXED(M22, sinNum);
    gCurrentMatrix[tbNum][6+2] = -MUL_FIXED(M21, sinNum) + MUL_FIXED(M22, cosNum);

    gRotregBuff[tbNum].matrix_a = gCurrentMatrix[tbNum][0];
    gRotregBuff[tbNum].matrix_b = gCurrentMatrix[tbNum][1];
    gRotregBuff[tbNum].matrix_c = gCurrentMatrix[tbNum][2];
    gRotregBuff[tbNum].matrix_e = gCurrentMatrix[tbNum][3+1];
    gRotregBuff[tbNum].matrix_f = gCurrentMatrix[tbNum][3+2];

    Zp =  gCurrentMatrix[tbNum][6+0] * (gRotregBuff[tbNum].viewp.x - gRotregBuff[tbNum].rotatecenter.x)
        + gCurrentMatrix[tbNum][6+1] * (gRotregBuff[tbNum].viewp.y - gRotregBuff[tbNum].rotatecenter.y)
        + gCurrentMatrix[tbNum][6+2] * (gRotregBuff[tbNum].viewp.z - gRotregBuff[tbNum].rotatecenter.z)
        + FIXED(gRotregBuff[tbNum].rotatecenter.z) + gRotateMoveZ[tbNum];

    Zs_ZpW = gCurrentMatrix[tbNum][6+0] * (- gRotregBuff[tbNum].viewp.x) + gCurrentMatrix[tbNum][6+2] * (- gRotregBuff[tbNum].viewp.z);

    if(gRotateDisp[tbNum])
    {
        gRotregBuff[tbNum].k_tab     = FIXED(300)+gRotregBuff[tbNum].screenst.y;
        gRotregBuff[tbNum].k_delta.x = Fcos(gRotateDisp[tbNum]);
        gRotregBuff[tbNum].k_delta.y = Fsin(gRotateDisp[tbNum]);
        gK_TableNum[tbNum] = 819;

        for(i=0; i < gK_TableNum[tbNum]; i++)
        {
            Zs_Zp = gCurrentMatrix[tbNum][6+1] * (i - 300  - gRotregBuff[tbNum].viewp.y)
                    + Zs_ZpW;

            if(Zs_Zp / 256)
            {
            if( (- DIV_FIXED(Zp,Zs_Zp) > FIXED(4) ) || (- DIV_FIXED(Zp,Zs_Zp) < 0) )
                gK_TableBuff[tbNum][i] = 0x8000;
            else
                gK_TableBuff[tbNum][i] = ((uint16_t)((- DIV_FIXED(Zp,Zs_Zp)) >> 6)) & 0x7fff;
            }
            else	gK_TableBuff[tbNum][i] = 0x8000;
        }
    } 
    else {
        if(gRbgKtbOffset[tbNum]!=-1 && gRbgKtbOffset[tbNum])
            gRotregBuff[tbNum].k_tab     = gRotregBuff[tbNum].screenst.y + FIXED(gRbgKtbOffset[tbNum]/2);
        else
            gRotregBuff[tbNum].k_tab     = FIXED(0)+gRotregBuff[tbNum].screenst.y;
            gRotregBuff[tbNum].k_delta.x = Fcos(FIXED(0));
            gRotregBuff[tbNum].k_delta.y = Fsin(FIXED(0));
            gK_TableNum[tbNum] = gDisplayY;

        for(i=0;i < gK_TableNum[tbNum];i++)
        {
            Zs_Zp = gCurrentMatrix[tbNum][6+1] * (i - gRotregBuff[tbNum].viewp.y) + Zs_ZpW;

            if(Zs_Zp / 256)
            {
            if( (- DIV_FIXED(Zp,Zs_Zp) > FIXED(4) ) || (- DIV_FIXED(Zp,Zs_Zp) < 0) )
                gK_TableBuff[tbNum][i] = 0x8000;
            else
                gK_TableBuff[tbNum][i] = ((uint16_t)((- DIV_FIXED(Zp,Zs_Zp)) >> 6)) & 0x7fff;
            }
            else	gK_TableBuff[tbNum][i] = 0x8000;
        }
    }

    gK_TableFlag[tbNum] = 1;

    return;
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_rotateY
 *
 * PARAMETERS
 *	param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - y Angle
 *
 * DESCRIPTION
 *
 * | M00 M01 M02 | |  cos 0  sin |
 * | M10 M11 M12 |.|    0 1    0 |
 * | M20 M21 M22 | | -sin 0  cos |
 *
 *			|  M00*cos-M02*sin  M01 M00*sin+M02*cos |
 *		    =	|  M10*cos-M12*sin  M11 M10*sin+M12*cos |
 *			|  M20*cos-M22*sin  M21 M20*sin+M22*cos |
 *
 *------------------------------------------------------------------------
 */
void  vdp2_rbg_rotateY(uint32_t screen, fix32_t yAngle)
{
    fix32_t		sinNum, cosNum;
    fix32_t		M00, M02, M10, M12, M20, M22;
    fix32_t		Zp,Zs_Zp,Zs_ZpW;
    uint32_t	i;
    uint16_t	tbNum;

    switch(screen)
    {
		case RBG_TB_A:
			tbNum = 0;
			g_r_reg.k_contrl &= 0xff00;
			g_r_reg.k_contrl |= 0x0003;
			break;
		case RBG_TB_B:
			tbNum = 1;
			g_r_reg.k_contrl &= 0x00ff;
			g_r_reg.k_contrl |= 0x0300;
			break;
		default:
			return;
			break;
    }

    sinNum = Fsin(yAngle);
    cosNum = Fcos(yAngle);
    M00 = gCurrentMatrix[tbNum][0+0];
    M02 = gCurrentMatrix[tbNum][0+2];
    M10 = gCurrentMatrix[tbNum][3+0];
    M12 = gCurrentMatrix[tbNum][3+2];
    M20 = gCurrentMatrix[tbNum][6+0];
    M22 = gCurrentMatrix[tbNum][6+2];
    gCurrentMatrix[tbNum][0+0] = MUL_FIXED(M00, cosNum) - MUL_FIXED(M02, sinNum);
    gCurrentMatrix[tbNum][0+2] = MUL_FIXED(M00, sinNum) + MUL_FIXED(M02, cosNum);
    gCurrentMatrix[tbNum][3+0] = MUL_FIXED(M10, cosNum) - MUL_FIXED(M12, sinNum);
    gCurrentMatrix[tbNum][3+2] = MUL_FIXED(M10, sinNum) + MUL_FIXED(M12, cosNum);
    gCurrentMatrix[tbNum][6+0] = MUL_FIXED(M20, cosNum) - MUL_FIXED(M22, sinNum);
    gCurrentMatrix[tbNum][6+2] = MUL_FIXED(M20, sinNum) + MUL_FIXED(M22, cosNum);

    gRotregBuff[tbNum].matrix_a = gCurrentMatrix[tbNum][0];
    gRotregBuff[tbNum].matrix_c = gCurrentMatrix[tbNum][2];
    gRotregBuff[tbNum].matrix_d = gCurrentMatrix[tbNum][3+0];
    gRotregBuff[tbNum].matrix_e = gCurrentMatrix[tbNum][3+1];
    gRotregBuff[tbNum].matrix_f = gCurrentMatrix[tbNum][3+2];

    Zp = gCurrentMatrix[tbNum][6+0]
	    * (gRotregBuff[tbNum].viewp.x - gRotregBuff[tbNum].rotatecenter.x) + gCurrentMatrix[tbNum][6+1]
	    * (gRotregBuff[tbNum].viewp.y - gRotregBuff[tbNum].rotatecenter.y) + gCurrentMatrix[tbNum][6+2]
	    * (gRotregBuff[tbNum].viewp.z - gRotregBuff[tbNum].rotatecenter.z) + FIXED(gRotregBuff[tbNum].rotatecenter.z) + gRotateMoveZ[tbNum];

    Zs_ZpW = gCurrentMatrix[tbNum][6+1] * (- gRotregBuff[tbNum].viewp.y) + gCurrentMatrix[tbNum][6+2] * (- gRotregBuff[tbNum].viewp.z);


    if(gRotateDisp[tbNum])
    {
		gRotregBuff[tbNum].k_tab     = FIXED(300)+gRotregBuff[tbNum].screenst.x;
		gRotregBuff[tbNum].k_delta.x = Fcos(gRotateDisp[tbNum]+FIXED(90) );
		gRotregBuff[tbNum].k_delta.y = Fsin(gRotateDisp[tbNum]+FIXED(90) );
		gK_TableNum[tbNum] = 819;

		for(i=0;i < gK_TableNum[tbNum];i++)
		{
			Zs_Zp = gCurrentMatrix[tbNum][6+0] * (i - 300 - gRotregBuff[tbNum].viewp.x) + Zs_ZpW;
			if(Zs_Zp / 256)
			{
			if( (- DIV_FIXED(Zp,Zs_Zp) > FIXED(4)) | (- DIV_FIXED(Zp,Zs_Zp) < 0) )
				gK_TableBuff[tbNum][i] = 0x8000;
			else
				gK_TableBuff[tbNum][i] = ((uint16_t)((- DIV_FIXED(Zp,Zs_Zp)) >> 6)) & 0x7fff;
			}
			else	gK_TableBuff[tbNum][i] = 0x8000;
		}
    }
    else 
    {
		gRotregBuff[tbNum].k_tab     = FIXED(0)+gRotregBuff[tbNum].screenst.x;
		gRotregBuff[tbNum].k_delta.x = Fcos(FIXED(90));
		gRotregBuff[tbNum].k_delta.y = Fsin(FIXED(90));
		gK_TableNum[tbNum] = gDisplayX;

		for(i=0;i < gK_TableNum[tbNum];i++)
		{
			Zs_Zp = gCurrentMatrix[tbNum][6+0] * (i - gRotregBuff[tbNum].viewp.x) + Zs_ZpW;
			if(Zs_Zp / 256)
			{
                if( (- DIV_FIXED(Zp,Zs_Zp) > FIXED(4)) | (- DIV_FIXED(Zp,Zs_Zp) < 0) )
                    gK_TableBuff[tbNum][i] = 0x8000;
                else
                    gK_TableBuff[tbNum][i] = ((uint16_t)((- DIV_FIXED(Zp,Zs_Zp)) >> 6)) & 0x7fff;
			}
			else	gK_TableBuff[tbNum][i] = 0x8000;
		}
    }

    gK_TableFlag[tbNum] = 1;

    return;
}


/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_rotateZ
 *
 * PARAMETERS
 *	param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - z Angle
 *
 * DESCRIPTION
 *
 * | M00 M01 M02 | | cos -sin  0 |
 * | M10 M11 M12 |.| sin  cos  0 |
 * | M20 M21 M22 | |   0    0  1 |
 *
 *				| M00*cos+M01*sin -M00*sin+M01*cos M02 |
 *			      =	| M10*cos+M11*sin -M10*sin+M11*cos M12 |
 *				| M20*cos+M21*sin -M20*sin+M21*cos M22 |
 *
 *
 *------------------------------------------------------------------------
 */
void  vdp2_rbg_rotateZ(uint32_t screen, fix32_t zAngle)
{
    fix32_t		sinNum, cosNum;
    fix32_t		M00, M01, M10, M11, M20, M21;
    uint16_t	tbNum;

    switch(screen)
    {
		case RBG_TB_A:
			tbNum = 0;
			break;
		case RBG_TB_B:
			tbNum = 1;
			break;
		default:
			return;
			break;
    }

    sinNum = Fsin(zAngle);
    cosNum = Fcos(zAngle);
    M00 = gCurrentMatrix[tbNum][0];
    M01 = gCurrentMatrix[tbNum][1];
    M10 = gCurrentMatrix[tbNum][3+0];
    M11 = gCurrentMatrix[tbNum][3+1];
    M20 = gCurrentMatrix[tbNum][6+0];
    M21 = gCurrentMatrix[tbNum][6+1];
    gCurrentMatrix[tbNum][0]   =  MUL_FIXED(M00, cosNum) + MUL_FIXED(M01, sinNum);
    gCurrentMatrix[tbNum][1]   = -MUL_FIXED(M00, sinNum) + MUL_FIXED(M01, cosNum);
    gCurrentMatrix[tbNum][3+0] =  MUL_FIXED(M10, cosNum) + MUL_FIXED(M11, sinNum);
    gCurrentMatrix[tbNum][3+1] = -MUL_FIXED(M10, sinNum) + MUL_FIXED(M11, cosNum);
    gCurrentMatrix[tbNum][6+0] =  MUL_FIXED(M20, cosNum) + MUL_FIXED(M21, sinNum);
    gCurrentMatrix[tbNum][6+1] = -MUL_FIXED(M20, sinNum) + MUL_FIXED(M21, cosNum);

    gRotregBuff[tbNum].matrix_a = gCurrentMatrix[tbNum][0];
    gRotregBuff[tbNum].matrix_b = gCurrentMatrix[tbNum][1];
    gRotregBuff[tbNum].matrix_d = gCurrentMatrix[tbNum][3+0];
    gRotregBuff[tbNum].matrix_e = gCurrentMatrix[tbNum][3+1];

    return;
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_scale
 *
 * PARAMETERS
 *  param1 - Rotation Scroll Screen (0 or 1)
 *	param2 - fix32_t	Sx:	X scale ratio
 *	param3 - fix32_t	Sy:	Y scale ratio
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_scale(uint32_t screen, fix32_t Sx, fix32_t Sy)
{
    fix32_t  wSx,wSy;

    wSx=Sx;
    wSy=Sy;

	wSx = DIV_FIXED(FIXED(1), wSx);
	wSy = DIV_FIXED(FIXED(1), wSy);

   	switch(screen)
   	{
		case RBG_TB_A:
			gRotregBuff[0].zoom.x = wSx;
			gRotregBuff[0].zoom.y = wSy;
			if( g_r_reg.k_contrl & 0x00ff )
			vdp2_rbg_rotate(screen, 0,0,0);
			break;

		case RBG_TB_B:
			gRotregBuff[1].zoom.x = wSx;
			gRotregBuff[1].zoom.y = wSy;
			if( g_r_reg.k_contrl & 0xff00 )
			vdp2_rbg_rotate(screen, 0,0,0);
			break;
   }
}



/*------------------------------------------------------------------------
 *
 * NAME : copyReg
 *
 * DESCRIPTION
 *		Copy parameters to rotation scroll H/W Registers
 *
 * CAVEATS
 * 		used by interrupt routine only(V_BLANK)
 *		***DON'T CALL IT DIRECTLY!***
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_copyReg()
{
	if(gK_TableFlag[0] && gRbgKtbAddr[0])	
    {
		vdp2_rbg_memcpyw((void *)gRbgKtbAddr[0],gK_TableBuff[0],gK_TableNum[0]*2);
		gK_TableFlag[0] = 0;
	}
    if(gK_TableFlag[1] && gRbgKtbAddr[1])	
    {
		vdp2_rbg_memcpyw((void *)gRbgKtbAddr[1],gK_TableBuff[1],gK_TableNum[1]*2);
		gK_TableFlag[1] = 0;
	}

    const	uint32_t	size = 0x60;	/*	sizeof(struct SclRotreg);	*/
    const	uint32_t	p= (uint32_t) gRotateTableAddress;
    void	*const	pA=(void *)p;
    void	*const	pB=(void *)(p+0x80);

    /*
    **  SclRotateTableAddress
    */
    if (pA != NULL)
    {
        vdp2_rbg_memcpyw(pA, &gRotregBuff[0], size);
        vdp2_rbg_memcpyw(pB, &gRotregBuff[1], size);
    }
	
    static	uint16_t * regaddr = (uint16_t *)0x25F80B0;
    vdp2_rbg_memcpyw(regaddr, &g_r_reg, sizeof(g_r_reg));
}

/*------------------------------------------------------------------------
 *
 * NAME : vdp2_rbg_memcpyw
 *
 * DESCRIPTION
 *		memory copy without DMA
 *
 *------------------------------------------------------------------------
 */
void vdp2_rbg_memcpyw(void *dest, void *src, uint32_t tcnt)	
{

	if((dest != NULL)&&(src != NULL))	
    {

		uint32_t tcr, tsize;
		tsize=tcnt;
		tsize=tsize/2;

		for (tcr = 0;tcr < tsize;tcr++)
        {
			*((uint16_t *)dest) = *((uint16_t *)src);
			dest = (uint8_t *)dest + 2;
			src = (uint8_t *)src + 2;
		}
	}
}