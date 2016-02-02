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
