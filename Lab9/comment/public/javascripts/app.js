angular.module('comment', []) 
.controller('MainCtrl', [ 
	'$scope','$http',
	 function($scope,$http){ 
		$scope.test = 'Hello world!';
		 
		$scope.comments = [ ];
		$scope.filter = [ ];
			/*{title:'Comment 1', upvotes:5}, 
			{title:'Comment 2', upvotes:6}, 
			{title:'Comment 3', upvotes:1}, 
			{title:'Comment 4', upvotes:4}, 
			{title:'Comment 5', upvotes:3} 
		];*/

		$scope.addComment = function() {
			//$scope.comments.push({title:$scope.formContent,upvotes:0});
			//$scope.formContent='';
			if($scope.formContent === '') { return; } 
			console.log("In addComment with "+$scope.formContent); 
			$scope.create({ 
				title: $scope.formContent, 
				upvotes: 0, 
			}); 
			$scope.formContent = '';
		};
		$scope.incrementUpvotes = function(comment) { 
			//comments.upvotes += 1;
			$scope.upvote(comment);
		};
		$scope.getAll = function() { 
			return $http.get('/comments').success(function(data){ 
				angular.copy(data, $scope.comments); 
			}); 
		};
		$scope.filterByVotes = function(){
			return $http.get('/comments').success(function(data){
				var temp = $scope.filterContent;
				$scope.filter = [];
				//console.log(temp);
				//$route.reload();
				data.sort(function(a,b) {return (a.upvotes > b.upvotes) ? 1 : ((b.upvotes > a.upvotes) ? -1 : 0);} ); 
				data.reverse();
				angular.forEach(data, function(key,value) {
					if(angular.isDefined(key.title)){
					if((key.title).indexOf(temp) > -1){
						this.push("***");
					}
					else {
						this.push(" ");
					}
					console.log("key: " + key.title + " Value: " + value); 
				}} ,$scope.filter); 
				console.log($scope.filter);
			});
		};
		$scope.getAll();
		$scope.create = function(comment) { 
			return $http.post('/comments', comment).success(function(data){ 
				$scope.comments.push(data); 
			}); 
		};
		$scope.upvote = function(comment) { 
			return $http.put('/comments/' + comment._id + '/upvote')
				.success(function(data){ 
					console.log("upvote worked"); 
					comment.upvotes += 1; 
				}); 
		};
	} 
]);

//$scope.comments = [ 'Comment 1', 'Comment 2', 'Comment 3', 'Comment 4', 'Comment 5' ];

