//
//  HomeViewController.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface HomeViewController : UIViewController

@property (nonatomic, weak) IBOutlet UIButton *startGameButton;
@property (nonatomic, weak) IBOutlet UIButton *gameOptionsButton;

@property (nonatomic, weak) IBOutlet UILabel *scoreValue;

-(IBAction)onStartGameButtonPressed:(id)sender;

@end
