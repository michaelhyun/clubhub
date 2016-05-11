class SessionsController < ActionController::Base
layout "users"
skip_before_action :authorize
  def new
  end
	
	def email
		session[:email] = @user.email
	end
	def password
		session[:password] = @user.password
	end
  
  def reset
    reset_session
	  redirect_to root_url
  end

#	def create
#		user = User.authenticate(params[:session][:email], params[:session][:password])
#		if user.nil?
#			flash.now[:error] = "Invalid email/password combination."
#			render :new
#		else
#			login user
#			redirect_to user
#	end
#  def create
#    user = User.find_by(email: params[:session][:email].downcase)
#    if user && user.authenticate(params[:session][:password])
#      log_in user
#      redirect_to user
#    else
#      flash[:danger] = 'Invalid email/password combination' # Not quite right!
#      render 'new'
#    end
#  end

  def destroy
    log_out
    redirect_to root_url
  end
end

