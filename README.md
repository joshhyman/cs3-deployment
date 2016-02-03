# CS3 Deployment

* followed directions at:
https://cloud.google.com/container-engine/docs/before-you-begin
  * `curl https://sdk.cloud.google.com | bash`
  * `gcloud components update kubectl`
* in container engine
  * created `cluster-1` with a single node
* defined some defaults
  * `gcloud config set project josh-cs3-2014`
  * `gcloud config set compute/zone us-central1-f` see also
    https://cloud.google.com/compute/docs/zones#available
  * `gcloud config set container/cluster cluster-1`
  * `gcloud container clusters get-credentials cluster-1`
* see defaults
  * `gcloud config list`
* install docker: https://docs.docker.com/engine/installation/
  * `sudo apt-get install docker-engine`
  * `sudo docker run hello-world`
* add a docker group:
  https://docs.docker.com/engine/installation/ubuntulinux/#create-a-docker-group
  * `sudo usermod -aG docker joshh`
  * logout and back in :(
* build a simple app, follow:
  https://cloud.google.com/container-engine/docs/tutorials/hello-node
  * write the Dockerfile
  * write a simple binary and build it
  * set env: `export PROJECT_ID=josh-cs3-2014`
  * build image: `docker build -t gcr.io/${PROJECT_ID}/hello-world:v1 .`
  * see that it built: `docker images`
  * run it locally: `docker run -i -t gcr.io/${PROJECT_ID}/hello-world:v1 /bin/bash`
* push the image
  * push image: `gcloud docker push gcr.io/${PROJECT_ID}/hello-world:v1`
* run the app
  * might already have something running: `gcloud container clusters list`
  * else: `gcloud container clusters create cluster-1 --num-nodes 1 --machine-type g1-small`
  * run the job: `kubectl run cluster-1 --image=gcr.io/${PROJECT_ID}/hello-node:v1 --port=8080`
  * allow traffic: `kubectl expose rc cluster-1 --type="LoadBalancer"`
  * check the IP: `kubectl get services cluster-1`
* debugging
  * find the node name: `kubectl get pods -o wide`
  * ssh to the node: `gcloud compute ssh $NODE`
  * see what containers ever ran: `sudo docker ps -a`
  * attempt to run your container: `sudo docker run foo`
* clean up
  * `kubectl delete services cluster-1`
  * `kubectl delete rc cluster-1`
  * `gcloud container clusters delete cluster-1`
