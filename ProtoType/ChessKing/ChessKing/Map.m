////
////  Map.m
////  ChessKing
////
////  Created by echo on 2014. 10. 14..
////  Copyright 2014년 echo. All rights reserved.
////
//
//#import "Map.h"
//
//bool isInsidePolygon(CGPoint *polygon,int N,CGPoint p)
//{
//    int counter = 0;
//    int i;
//    double xinters;
//    CGPoint p1,p2;
//    
//    p1 = polygon[0];
//    for (i=1;i<=N;i++) {
//        p2 = polygon[i % N];
//        if (p.y > MIN(p1.y,p2.y)) {
//            if (p.y <= MAX(p1.y,p2.y)) {
//                if (p.x <= MAX(p1.x,p2.x)) {
//                    if (p1.y != p2.y) {
//                        xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
//                        if (p1.x == p2.x || p.x <= xinters)
//                            counter++;
//                    }
//                }
//            }
//        }
//        p1 = p2;
//    }
//    
//    if (counter % 2 == 0)
//        return(false);
//    else
//        return(true);
//}
//
//CGPoint getCenterPosition(int x, int y)
//{
//    CGPoint centerPoint;
//    centerPoint.x = (1.f * powf(3.f, 0.5f) * MAP_RADIUS * cosf(M_PI / 6.f) * x);
//    centerPoint.y = ( powf(3.f , 0.5f) * MAP_RADIUS * y) - (powf(3.f, 0.5f) * MAP_RADIUS * sinf(M_PI / 6.f) * x) ;
//    return centerPoint;
//}
//
//CGPoint getCenterPositionWithHexaPoint(CGPoint hexaPoint)
//{
//    return getCenterPosition(hexaPoint.x, hexaPoint.y);
//}
//
//CGPoint getHexaPointByScreenPoint(CGPoint point)
//{
//    for(int x = 0 ;  x < MAP_WIDTH; x++)
//    {
//        for(int y = 0 ; y < MAP_HEIGHT; y++)
//        {
//            CGPoint centerPoint = getCenterPosition(x, y);
//            CGPoint verts[6];
//            for( int i = 0 ; i < 6; i++)
//            {
//                verts[i] = ccp(centerPoint.x + cosf(M_PI / 3.f * i) * MAP_RADIUS,
//                               centerPoint.y + sinf(M_PI / 3.f * i) * MAP_RADIUS );
//            }
//            if(isInsidePolygon(verts, 6, point))
//                return ccp(x,y);
//        }
//    }
//    return ccp(-1,-1);
//}
//
//bool isMovalbeAction(CGPoint startHexaPoint, int velocity, CGPoint wannaGoHexaPoint)
//{
//    CGFloat dX[6] = {-1, 0, 1, 1, 0, -1,};
//    CGFloat dY[6] = {-1, -1, 0, 1, 1, 0,};
//    
//    for( int i = 0 ; i < 6; i++) // index for Angle
//    {
//        for( int v = 1 ; v <= velocity; v++)
//        {
//            CGPoint point = ccp( startHexaPoint.x + dX[i] * v,
//                                startHexaPoint.y + dY[i] * v);
//            if(CGPointEqualToPoint(wannaGoHexaPoint, point))
//                return true;
//        }
//    }
//    return false;
//}
//
//@implementation Map
//-(id)init
//{
//    if ( self = [super init])
//    {
//        mapNodeList = [NSMutableArray array];
//        [self generateMapByHardCoding];
//        
//        
//        
//        for( int x = 0 ; x < MAP_WIDTH; x++)
//        {
//            for( int y = 0 ; y < MAP_HEIGHT; y++)
//            {
//                CGPoint centerPoint = getCenterPosition(x, y);
//                for( int i = 0 ; i < 6; i++)
//                {
//                    verts[x][y][i] = ccp(centerPoint.x + cosf(M_PI / 3.f * i) * MAP_RADIUS,
//                                         centerPoint.y + sinf(M_PI / 3.f * i) * MAP_RADIUS );
//                }
//                
//                if([self isEnablePosition:ccp(x,y)])
//                {
//                    [self drawPolyWithVerts:verts[x][y] count:6 fillColor:[CCColor colorWithCcColor4f:ccc4f(0.f, 0.f, 0.f, 0.f)] borderWidth:1.f borderColor:[CCColor colorWithCcColor4f:ccc4f(0.f, 1.f, 0.f, 1.f)] ];
//                    
//#ifdef HEXA_INDEX_SHOW
//                    // Label for Coord
//                    CCLabelTTF* label = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d,%d",x,y] fontName:@"Verdana"fontSize:20.f];
//                    label.position = centerPoint;
//                    [self addChild:label];
//#endif
//                }
//            }
//        }
//    }
//    return self;
//}
//
//-(void)generateMapByHardCoding
//{
//    int maxY = 7;
//    int maxX = 9;
//    int startX = 3;
//    int startY = 3;
//    for( int x = 0 ; x < maxX; x ++)
//    {
//        if(x%2 == 0)
//        {
//            for( int y = 0 ; y < maxY; y++)
//            {
//                CGPoint point =  ccp(startX + x,
//                                   startY + y + (x%2));
//                [mapNodeList addObject:[NSValue valueWithCGPoint:point]];
//            }
//        }
//        else
//        {
//            for( int y = 0; y < maxY - 1; y++)
//            {
//                CGPoint point = ccp(startX + x,
//                                    startY + y + (x%2));
//                [mapNodeList addObject:[NSValue valueWithCGPoint:point]];
//            }
//        }
//    }
//}
//
//-(bool)isEnablePosition:(CGPoint)hexaPoint
//{
//    for (NSValue *value in mapNodeList) {
//        CGPoint point = [value CGPointValue];
//        if(CGPointEqualToPoint(point, hexaPoint))
//            return true;
//    }
//    return false;
//}
//@end
