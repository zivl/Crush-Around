//
//  NotificationView.m
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/5/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import "NotificationView.h"

@implementation NotificationView

@synthesize bg;
@synthesize message;
@synthesize okButton;

- (id)init
{
    self = [super init];
    if (self) {
		NSArray *nibs = [[NSBundle mainBundle] loadNibNamed:@"NotificationView" owner:nil options:nil];
		self = [nibs objectAtIndex:0];
		self.alpha = 0.0f;
		self.okButton.titleLabel.text = @"OK";
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(void)showNotificationWithMessage:(NSString *)iMessgae {
	self.message.text = iMessgae;
	[UIView animateWithDuration:0.5 animations:^{
		self.alpha = 1.0f;
	} completion:nil];
}

-(void)hideNotificationMessage {
	[UIView animateWithDuration:0.5 animations:^{
		self.alpha = 0.0f;
	} completion:^(BOOL finished){
		if(finished){
			self.message.text = @"";
		}
	}];
}

-(IBAction)onOKButtonPressed:(id)sender {
	[self hideNotificationMessage];
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
