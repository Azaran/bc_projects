using IW5Project.Model.Movies;

namespace IW5Project.Model.Persons
{
    public class PersonMovieRole: BaseModel
    {
        public enum Role { Actor, Director }

        public virtual Person Person { get; set; }
        public virtual Movie Movie { get; set; }
        public Role WorkRole { get; set; }
    }
}