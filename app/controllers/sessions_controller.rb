class SessionsController < ActionController::Base
layout "users"
skip_before_action :authorize
  def new
  end
	def create
<<<<<<< HEAD
    	user = User.find_by(email: params[:session][:email].downcase)
   		if user && user.authenticate(params[:session][:password])
    	session[:user_id] = user.id
    	redirect_to user
=======
    user = User.find_by(email: params[:session][:email].downcase)
   if user && user.authenticate(params[:session][:password])
    session[:user_id] = user.id
    redirect_to user
>>>>>>> origin/ka-branch
		else
      render 'new'
	  end
  end

<<<<<<< HEAD
=======

>>>>>>> origin/ka-branch
  def destroy
    session.delete(:user_id)
    @current_user = nil
    redirect_to root_url
  end
<<<<<<< HEAD

=======
>>>>>>> origin/ka-branch

end
