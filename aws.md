* setup an AWS account
  * http://docs.aws.amazon.com/AmazonECS/latest/developerguide/get-set-up-for-amazon-ecs.html
  * click setup free account
  * sign in if you have an amazon account, make one if you dont
  * create a personal account
  * enter a CC-number
  * verify phone number
  * select the basic plan
* sign into console
  * https://console.aws.amazon.com/
* create an IAM user
  * https://console.aws.amazon.com/iam/
  * follow instructions on setup guide for ECS
* install AWS CLI
  * `apt-get install python-pip`
  * `pip install awscli`
  * generate key in the console
  * http://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-started.html
* setup ECS
  * http://docs.aws.amazon.com/AmazonECS/latest/developerguide/ECS_GetStarted.html
  * https://console.aws.amazon.com/ecs/home#/firstRun
  * `aws ecr get-login --region us-west-2`
* build and push container
  * write the Dockerfile
  * write a simple binary and build it
  * build image: `docker build -t cs3 .`
  * see that it built: `docker images`
  * run it locally: `docker run -i -t cs3 /bin/bash`
  * tag it: `docker tag cs3:latest 640141239503.dkr.ecr.us-west-2.amazonaws.com/[repo]:latest`
  * push it: `docker push 640141239503.dkr.ecr.us-west-2.amazonaws.com/[repo]:latest`
* run it on ECS
  * create cluster
  * view cluster and go to ECS instances
  * select Amazon Linux AMI
  * select t2.micro
  * create task definition (see json file)
  * click actions, run task, select the proper cluster and click run

