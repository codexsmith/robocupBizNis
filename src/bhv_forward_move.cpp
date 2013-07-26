// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "shared.h"

#include "bhv_forward_move.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_ForwardMove::execute( PlayerAgent * agent )
{
    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove" );

    //-----------------------------------------------
    // tackle
    if ( Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )
    {
        return true;
    }

    const WorldModel & wm = agent->world();
    /*--------------------------------------------------------*/
    // chase ball
    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( ! wm.existKickableTeammate()
         && ( self_min <= 3
              || ( self_min <= mate_min
                   && self_min < opp_min + 3 )
              )
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        Body_Intercept().execute( agent );
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;
    }

    Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    
    //BIZNIS
    char triangle = 'T';
    if(Shared::getCommand() == &triangle){
      if(DEBUG){  
      std::cout << "forward triangle" <<std::endl;
      }
        const PlayerObject* on_ball = wm.getTeammateNearestToBall(3,false);
        Vector2D on_ball_position = on_ball->pos();
        
	if(on_ball->unum() != wm.self().unum()){
	
        Vector2D offsetY = Vector2D(7, 0);
	Vector2D offsetX = Vector2D(0,7);
        
        bool attackRight = false;
        
        if(wm.getOpponentGoalie()->pos().x > wm.getOurGoalie()->pos().x){
            attackRight = true;
        }
        
        //calculates closest position in a triangle formation behind the ball holder
        if(on_ball_position.y > wm.self().pos().y){
            if(attackRight){
		(offsetY * -1) - offsetX;
		on_ball_position.operator+=(offsetY);
		target_point = on_ball_position;
	      }
	      else{//attackLeft
		(offsetY * -1) + offsetX;
		target_point = on_ball_position + offsetY;
	      }
            }
            else{//ball.Y < my.Y
	      if(attackRight){
		offsetY - offsetX;
		target_point = on_ball_position + offsetY;
	      }
	      else{//attackLeft
		offsetY + offsetX;
		target_point = on_ball_position + offsetY;
	      }
	  }
	}
// 	Strategy::i().setTargetPosition(wm.self().unum(), target_point.x, target_point.y);
	
    }
    
    const double dash_power = Strategy::get_normal_dash_power( wm );

    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
    }

    if ( wm.existKickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new Neck_TurnToBall() );
    }
    else
    {
        agent->setNeckAction( new Neck_TurnToBallOrScan() );
    }

    return true;
}

