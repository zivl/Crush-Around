//
//  HomeViewController.m
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import "HomeViewController.h"
#import "GameConfiguration.h"

@interface HomeViewController ()

@end

@implementation HomeViewController

@synthesize startGameButton;
@synthesize gameOptionsButton;
@synthesize scoreValue;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

	UIFont *font = [UIFont fontWithName:@"GROBOLD" size:18.0f];
	[self.scoreValue setFont: font];
	NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
	self.scoreValue.text = [NSString stringWithFormat:@"%ld", (long)[storage integerForKey:SCORE_KEY]];

}

-(IBAction)onStartGameButtonPressed:(id)sender {

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
