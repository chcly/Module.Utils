pipeline {
    agent any
    triggers {
        GenericTrigger(causeString: 'Relay Push Service',  token: 'Template.Build')
    }
    stages {
        stage('Build Project') {

            steps {
                sh script: 'cmake -DTemplate_BUILD_TEST=ON -DTemplate_AUTO_RUN_TEST=ON .'
                sh script: 'make'
            }
        }

        stage('Test Project') {
            steps {
                sh script: './Source/Template > out.ans'
                sh script: 'diff out.ans Test/inp.ans'
            }
        }

        stage('Finalize') {
            steps {
                deleteDir()
            }
        }
    }
}